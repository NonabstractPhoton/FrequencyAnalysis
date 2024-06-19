#include "Header.h"
#include "wavreader.cpp"
#include "decoder.cpp"
#include "helperfuncs.cpp"

#include <iostream>
#include <fstream>


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

    
    if (argc < 3)
    {
        std::cerr << "Usage: Analyzer <in.wav> <out.csv>" << std::endl;
        return -1;
    }

    std::ofstream file;
    file.open(argv[2]);
    

    std::vector<double> data;

    readWav_float32(argv[1], &data);

    std::cout << "Size: " << data.size() << std::endl;
    
    
    
    int N = data.size();
    double T = 2;
    fftw_complex *out;
    fftw_plan p;
    
    out = fftw_alloc_complex(N/2+1);
    
    p = fftw_plan_dft_r2c_1d(N, data.data(), out, FFTW_ESTIMATE);
    
    fftw_execute(p);

    coefsToAmplitudes(out, N);

    file << "Frequency,Amplitude\n";

    // Only up until 30khz frequencies
    const int frequenciesLimit = std::min(N/2+1, 60000);
    
    for (int i = 0; i < frequenciesLimit; i++)
    {
        file << i / (double)T << ',' << out[i][0] << "\n";
    }

    /*
    matplot::bar(x, y);
    matplot::axis(matplot::automatic);
    matplot::show();
    */
   
    file.close();
    fftw_destroy_plan(p);
    fftw_free(out);
    

    return 0;
}