<h1>Overview</h1>

This command line application generates a frequency graph of audible frequencies from a given .wav file. It also identifies musical sharps and flats in an attempt to identify the musical key of the audio, which works best if the music is modal. This project is still in alpha, however is fully functional for the described use cases.


<h1>Methods</h1>

The C++ portion of this application uses a Fast Fourier Transform to extract Frequency vs Amplitude information from the provied 32 bit float .wav file. The codebase contains a basic framework for other formats, however these are yet to be implemented. The application then compares relative frequency ocurrence to determine musical accidentals (sharps and flats), and stores all data to an out.data file. Finally, the C# portion of this application graphs that data using ScottpPlot and outputs the musical key information.  


<h1>Installation</h1>

Download all files from the Release directory and run FrequencyGrapher.exe while providing your input audio .wav as an argument. If one so chooses, they can also directly use the Analyzer.exe by providing an input .wav file and the path for an out.data file which will contain frequency vs amplitude information and the detected musical accidentals.
