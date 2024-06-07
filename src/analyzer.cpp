#include "Header.h"
#include "decoder.cpp"

 
#define PI 3.14159265

double f(double t)
{
    return 2 * sin(2 * PI * t) + 3 * sin(2 * PI * 4 * t) + sin(2 * PI * 16 * t);
}

void coefsToAmplitudes(fftw_complex* coefs, int N)
{
    coefs[0][0] = sqrt(coefs[0][0] * coefs[0][0] + coefs[0][1] * coefs[0][1]) / (double)N;

    for (int i = 1; i < N / 2; i++)
    {
        coefs[i][0] = 2 * sqrt(coefs[i][0] * coefs[i][0] + coefs[i][1] * coefs[i][1]) / (double)N;
    }

    coefs[N/2][0] = (N % 2 == 0 ? 1 : 2) * sqrt(coefs[N / 2][0] * coefs[N / 2][0] + coefs[N / 2][1] * coefs[N / 2][1]) / (double)N;
}

int main(int argc, char const *argv[])
{

    std::cout << "Running\n";

    
    if (argc < 2)
    {
        std::cerr << "Please provide the path to an audio file as an argument." << std::endl;
        return -1;
    }

    constexpr int sampleRate = 44100;
    double* data;
    int size;

    decode_audio_file(argv[1], sampleRate, &data, &size);

    std::cout << "Size: " << size;
    
    /*
    int N = 100;
    double T = 2;

    double *in, *freqs;
    fftw_complex *out;
    fftw_plan p;
    
    in = fftw_alloc_real(N);
    out = fftw_alloc_complex(N/2+1);
    freqs = fftw_alloc_real(N/2+1);
    
    p = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);

    for (int i = 0; i < N; i++)
    {
        in[i] = f(i / (double)N * T);
    }
    
    fftw_execute(p);

    coefsToAmplitudes(out, N);


    for (int i = 0; i < N / 2 + 1; i++)
    {
        freqs[i] = i / (double)T;
    }

    std::cout << "Frequency vs Amplitude:\n";

    std::vector<double> x;
    std::vector<double> y;

    for (int i = 0; i < N / 2 + 1; i++)
    {
        x.push_back(freqs[i]);
        y.push_back(out[i][0]);
        std::cout << freqs[i] << ", " << out[i][0] << "\n";
    }

    matplot::bar(x, y);
    matplot::show();
    
    fftw_destroy_plan(p);
    fftw_free(in); fftw_free(out); fftw_free(freqs);
    */

    return 0;
}
