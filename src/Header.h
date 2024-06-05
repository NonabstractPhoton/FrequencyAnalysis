#pragma once
#include <stdlib.h>
#include <iostream>
#include <fftw3.h>
#include <matplot/matplot.h>
extern "C" 
{
    #include <libavcodec/avcodec.h>
    #include <libavutil/opt.h>
    #include <libavformat/avformat.h>
    #include <libswresample/swresample.h>
}
