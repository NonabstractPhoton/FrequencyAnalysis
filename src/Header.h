#pragma once
#include <stdlib.h>
#include <iostream>
#include <fftw3.h>
#include <matplot/matplot.h>
extern "C" 
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/opt.h>
    #include <libavutil/imgutils.h>
    #include <libswresample/swresample.h>
}
