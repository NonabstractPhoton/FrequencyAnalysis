#include "Header.h"

int decode_audio_file(const char* path, const int sample_rate, double** data, int* size) {

    // get format from audio file
    AVFormatContext* format = avformat_alloc_context();
    if (avformat_open_input(&format, path, NULL, NULL) != 0) {
        fprintf(stderr, "Could not open file '%s'\n", path);
        return -1;
    }
    if (avformat_find_stream_info(format, NULL) < 0) {
        fprintf(stderr, "Could not retrieve stream info from file '%s'\n", path);
        return -1;
    }

    // Find the index of the first audio stream
    int stream_index = -1;
    for (int i = 0; i < format->nb_streams; i++) {
        if (format->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            stream_index = i;
            break;
        }
    }
    if (stream_index == -1) {
        fprintf(stderr, "Could not retrieve audio stream from file '%s'\n", path);
        return -1;
    }
    AVStream* stream = format->streams[stream_index];

    // find & open codec
    auto codec = avcodec_find_decoder(stream->codecpar->codec_id);
    if (!codec)
    {
        std::cerr << "Could not find avcodec decoder";
        return -1;
    }
    auto context = avcodec_alloc_context3(codec);
    if (avcodec_parameters_to_context(context, stream->codecpar) < 0)
    {
        std::cerr << "Could not populate context with params";
        return -1;
    }
    if (avcodec_open2(context, codec, NULL) < 0) {
        fprintf(stderr, "Failed to open decoder for stream #%u in file '%s'\n", stream_index, path);
        return -1;
    }

    // prepare resampler
    struct SwrContext* swr = swr_alloc();
    av_opt_set_int(swr, "user_in_ch_count", context->ch_layout.nb_channels, 0);
    av_opt_set_int(swr, "user_out_ch_count", 1, 0);
    av_opt_set_int(swr, "user_in_ch_layout", context->ch_layout.nb_channels, 0);
    av_opt_set_int(swr, "user_out_ch_layout", AV_CH_LAYOUT_MONO, 0);
    av_opt_set_int(swr, "in_sample_rate", stream->codecpar->sample_rate, 0);
    av_opt_set_int(swr, "out_sample_rate", sample_rate, 0);
    av_opt_set_sample_fmt(swr, "user_in_sample_fmt", context->sample_fmt, 0);
    av_opt_set_sample_fmt(swr, "user_out_sample_fmt", AV_SAMPLE_FMT_DBL, 0);
    swr_init(swr);
    if (!swr_is_initialized(swr)) {
        fprintf(stderr, "Resampler has not been properly initialized\n");
        return -1;
    }

    // prepare to read data
    AVPacket packet;
    av_init_packet(&packet);
    AVFrame* frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Error allocating the frame\n");
        return -1;
    }

    // iterate through frames
    *data = NULL;
    *size = 0;
    while (av_read_frame(format, &packet) >= 0) {
        // decode one frame - DEPPRECATED, USE RECEIVE 
        int ret = avcodec_send_packet(context, &packet);
        if (ret < 0)
        {
            std::cerr << "Error in decoding: " << ret;
        }

        while (ret >= 0)
        {
            avcodec_receive_frame(context, frame);
            // resample frames
            double* buffer;
            av_samples_alloc((uint8_t**)&buffer, NULL, 1, frame->nb_samples, AV_SAMPLE_FMT_DBL, 0);
            int frame_count = swr_convert(swr, (uint8_t**)&buffer, frame->nb_samples, (const uint8_t**)frame->data, frame->nb_samples);
            // append resampled frames to data
            *data = (double*)realloc(*data, (*size + frame->nb_samples) * sizeof(double));
            memcpy(*data + *size, buffer, frame_count * sizeof(double));
            *size += frame_count;
            av_freep(&buffer[0]);
        }


    }

    // clean up
    av_frame_free(&frame);
    av_packet_unref(&packet);
    swr_free(&swr);
    avformat_free_context(format);
    avcodec_free_context(&context);

    // success
    return 0;

}