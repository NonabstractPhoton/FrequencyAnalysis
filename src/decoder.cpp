#include "Header.h"

int readWav(const char* path, std::vector<double>* amplitudeData) {
    
    AVFormatContext* formatContext = nullptr;
    int audioStreamIndex = -1;

    // Initialize FFmpeg
    avformat_network_init();

    // Open the audio file
    if (avformat_open_input(&formatContext, path, nullptr, nullptr) != 0) {
        std::cerr << "Could not open file " << path << std::endl;
        return -1;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        std::cerr << "Could not find stream information" << std::endl;
        avformat_close_input(&formatContext);
        return -1;
    }

    // Find the audio stream
    for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStreamIndex = i;
            break;
        }
    }

    if (audioStreamIndex == -1) {
        std::cerr << "Could not find audio stream" << std::endl;
        avformat_close_input(&formatContext);
        return -1;
    }

    AVCodecParameters* codecParameters = formatContext->streams[audioStreamIndex]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecParameters->codec_id);
    if (!codec) {
        std::cerr << "Unsupported codec!" << std::endl;
        avformat_close_input(&formatContext);
        return -1;
    }

    AVCodecContext* codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        std::cerr << "Could not allocate audio codec context" << std::endl;
        avformat_close_input(&formatContext);
        return -1;
    }

    if (avcodec_parameters_to_context(codecContext, codecParameters) < 0) {
        std::cerr << "Could not copy codec parameters to context" << std::endl;
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return -1;
    }

    if (avcodec_open2(codecContext, codec, nullptr) < 0) {
        std::cerr << "Could not open codec" << std::endl;
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return -1;
    }

    AVPacket* packet = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();

    // Read and decode audio frames
    while (av_read_frame(formatContext, packet) >= 0) {
        if (packet->stream_index == audioStreamIndex) {
            if (avcodec_send_packet(codecContext, packet) == 0) {
                while (avcodec_receive_frame(codecContext, frame) == 0) {
                    // Assuming the audio is in AV_SAMPLE_FMT_S16 format
                    if (codecContext->sample_fmt == AV_SAMPLE_FMT_S16) {
                        int data_size = av_get_bytes_per_sample(codecContext->sample_fmt);
                        for (int i = 0; i < frame->nb_samples; i++) {
                            for (int ch = 0; ch < codecContext->ch_layout.nb_channels; ch++) {
                                int16_t* sample = (int16_t*)frame->data[ch];
                                amplitudeData->push_back((double)sample[i]);
                            }
                        }
                    } else {
                        std::cerr << "Unsupported sample format!" << std::endl;
                    }
                }
            }
        }
        av_packet_unref(packet);
    }

    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);

    return 0;
}