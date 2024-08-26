#include "Header.h"
#include "wavreader.cpp"
#include "decoder.cpp"

#include <fftw3.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <filesystem>

#define USAGE_ERROR_EXIT_IF(condition) if (condition) {std::cerr << "Usage: Analyzer <in.wav> <out.data> or <path/to/formatted audio directory> <path/to/data output directory>" << std::endl; return -1; }


// Ampltiude of compelx number, normalizationf actor of 2/N for all but DC and Nyquist
void coefsToAmplitudes(fftw_complex* coefs, int N)
{
    // Magnitude/N 
    coefs[0][0] = sqrt(coefs[0][0] * coefs[0][0] + coefs[0][1] * coefs[0][1]) / (double)N;

    for (int i = 1; i < N / 2; i++)
    {
        // 2 * Magnitude / N
        coefs[i][0] = 2 * sqrt(coefs[i][0] * coefs[i][0] + coefs[i][1] * coefs[i][1]) / (double)N;
    }
    // Magnitude/N if even else 2 * Magnitude/N
    coefs[N/2][0] = (N % 2 == 0 ? 1 : 2) * sqrt(coefs[N / 2][0] * coefs[N / 2][0] + coefs[N / 2][1] * coefs[N / 2][1]) / (double)N;
}


void analyzeSingleWav(const char* inFile, const char* outFile)
{
    std::vector<double> data;

    readWav_float32(inFile, &data);    
    
    constexpr int samplingRate = 44100;
    
    if (data.size() > samplingRate)
        data.erase(data.end()-data.size() % samplingRate, data.end());
    else
    {
        std::cerr << "Audio file too short. Skipping " << std::string{inFile} << std::endl;
        return;
    }

    std::ofstream file;
    file.open(outFile);

    const int N = data.size();
    // Rounding should be redundant, left in for edge cases of short durations
    const double T = std::round(data.size()/samplingRate);
    fftw_complex *out;
    fftw_plan p;
    
    out = fftw_alloc_complex(N/2+1);
    
    p = fftw_plan_dft_r2c_1d(N, data.data(), out, FFTW_ESTIMATE);
    
    fftw_execute(p);

    coefsToAmplitudes(out, N);
    
    // Only up until 20khz frequencies
    constexpr int maxFrequency = 20000;
    const int frequenciesLimit = std::min(N/2+1, (int)(maxFrequency * T));

    file << (frequenciesLimit/T) << "\n";

    // Skipping to bins 1 Hz apart
    for (int i = T; i <= frequenciesLimit; i+=T)
    {
        double freq = i / T;
        file << freq << ',' << out[i][0] << "\n";
    }
   
    file.close();
    fftw_destroy_plan(p);
    fftw_free(out);
}

void analyzeManyWav(const char* inDir, const char* outDir)
{
    std::fstream file;
    std::stringstream inFileName, outFileName;

    std::string inDirStr{inDir};
    std::string outDirStr{outDir};

    if (!inDirStr.ends_with("\\"))
        inDirStr.append("\\");

    if (!outDirStr.ends_with("\\"))
        outDirStr.append("\\");

    inFileName << inDirStr << "count.txt";

    try 
    {
        file.open(inFileName.str());
    }
    catch (const std::exception& e) 
    {
        std::cerr << e.what();
        file.close();
        return;
    }
    

    std::string line;
    std::getline(file, line);

    const int count = std::stoi(line);

    file.close();
    inFileName.str(std::string());
    inFileName.clear();

    for (int i = 0; i < count; i++)
    {
        inFileName << inDirStr << i << ".wav";
        outFileName << outDirStr << i << ".data";

        analyzeSingleWav(inFileName.str().c_str(), outFileName.str().c_str());

        inFileName.str(std::string());
        inFileName.clear();

        outFileName.str(std::string());
        outFileName.clear();
    }
}

int main(int argc, char const *argv[])
{
    USAGE_ERROR_EXIT_IF(argc < 2)    

    std::string out = std::string{ argv[2] };

    // Must be a .data file or a valid directory
    USAGE_ERROR_EXIT_IF
    (
        !out.ends_with(".data")
        && 
        out.compare(".") != 0
        &&
        out.find("\\") == std::string::npos 
        &&
        out.find("/") == std::string::npos
    );

    if (out.ends_with(".data"))
    {
        analyzeSingleWav(argv[1], argv[2]);
    }
    else
    {
        analyzeManyWav(argv[1], argv[2]);
    }

    return 0;
}
