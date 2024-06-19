#include "Header.h"
#include "wavreader.cpp"
#include "decoder.cpp"
#include "helperfuncs.cpp"

#include <fftw3.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <map>


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
    
    if (argc < 3)
    {
        std::cerr << "Usage: Analyzer <in.wav> <out.data>" << std::endl;
        return -1;
    }

    std::ofstream file;
    file.open(argv[2]);
    

    std::vector<double> data;

    readWav_float32(argv[1], &data);    
    
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

    file << frequenciesLimit << "\n";
    // A0 - A9, 27.5 Hz to 28160 hz
    constexpr int lowestOctave = -4, highestOctave = 6;
    constexpr int semitoneCount = (highestOctave-lowestOctave) * 12; 
    constexpr double A4 = 440;
    
    double freqBarriers[semitoneCount+1];
    
    for (int i = 0; i < semitoneCount+1; i++)
    {
        freqBarriers[i] = A4 * std::pow(2, (lowestOctave*12 + i)/12.0);
    }

    int noteAmplitudes[12];
    int currentIndex = 0;

    for (int i = 0; i < frequenciesLimit; i++)
    {
        double freq = i / (double)T;

        if (currentIndex < semitoneCount)
        {
            if (freq > (freqBarriers[currentIndex] + freqBarriers[currentIndex+1]) / 2.0)
                currentIndex++;

            noteAmplitudes[currentIndex % 12] += out[i][0];
        }
        
        file << freq << ',' << out[i][0] << "\n";
    }


    std::map<int, std::string> semitoneToSharp 
    {
        {1, "A#"},
        {3, "B#"},
        {4, "C#"},
        {6, "D#"},
        {8, "E#"},
        {9, "F#"},
        {11, "G#"}
    };

    std::map<int, std::string> semitoneToFlat 
    {
        {1, "Bb"},
        {2, "Cb"},
        {4, "Db"},
        {6, "Eb"},
        {7, "Fb"},
        {9, "Gb"},
        {11, "Ab"}
    };

    bool sharps[5];
    int sharpCheck[] = {0, 3, 5, 8, 10};
    file << "Sharps: ";
    for (int i = 0; i < 5; i++)
    {
        if (noteAmplitudes[sharpCheck[i]+1] > noteAmplitudes[sharpCheck[i]])
        {
            sharps[i] = true;
            file << semitoneToSharp[sharpCheck[i]+1] << " ";
        }

    }

    bool flats[6];
    int flatCheck[] = {0,2,3,5,7,10};
    file << "\nFlats: ";

    if (noteAmplitudes[11] > noteAmplitudes[0])
    {
        flats[0] = true;
        file << semitoneToFlat[11] << " ";
    }

    for (int i = 1; i < 6; i++)
    {
        if (noteAmplitudes[flatCheck[i]-1] > noteAmplitudes[flatCheck[i]])
        {
            flats[i] = true;
            file << semitoneToFlat[flatCheck[i]-1] << " ";
        }
    }
   
    file.close();
    fftw_destroy_plan(p);
    fftw_free(out);
    

    return 0;
}