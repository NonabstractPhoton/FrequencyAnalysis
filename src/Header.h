#pragma once
#include <stdlib.h>
#include <fftw3.h>
#include <vector>
extern "C" 
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/opt.h>
    #include <libavutil/imgutils.h>
    #include <libswresample/swresample.h>
}