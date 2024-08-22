using ScottPlot;
using System.Diagnostics;
using System.Text.RegularExpressions;

namespace FrequencyGrapher
{
    internal class Program
    {
        const int xDim = 1920, yDim = 1080;
        const int freqsPerPlot = 50;

        static void Main(string[] args)
        {

            if (args.Length != 1)
            {
                Console.WriteLine("Please Provide an input wav file as an argument.");
                Environment.Exit(-1);
            }
            string inputPath = Path.GetFullPath(args[0]);
            string directory = inputPath.Replace(args[0], "");

            var proc = Process.Start("Analyzer.exe", new string[] { inputPath, directory + "out.data" });
            proc.WaitForExit();
            if (proc.ExitCode != 0) 
            {
                Environment.Exit(proc.ExitCode);
            }

            GenerateGraph(directory);
        }

        static void GenerateGraph(string pathToData)
        {
            Plot p = new();

            p.Title("Frequency vs Amplitude");

            string sharpInfo, flatInfo;

            using (StreamReader reader = new(pathToData+"out.data"))
            {
                double entries = double.Parse(reader.ReadLine());

                double[] freqs = new double[freqsPerPlot], amplitudes = new double[freqsPerPlot];

                int entryIndex = 0;

                while(entryIndex < entries) 
                {
                    for (int i = 0; i < freqsPerPlot; i++)
                    {
                        var data = reader.ReadLine().Split(",");
                        freqs[i] = double.Parse(data[0]);
                        amplitudes[i] = double.Parse(data[1]);
                    }

                    var plt = p.Add.Bars(freqs, amplitudes);

                    entryIndex += freqsPerPlot;
                }
                sharpInfo = reader.ReadLine();
                flatInfo = reader.ReadLine();
                Console.WriteLine($"{sharpInfo}\n{flatInfo}");
            }

            p.SavePng(pathToData + "Plot.png", xDim, yDim);
        }
    }
}
