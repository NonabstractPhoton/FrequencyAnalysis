using ScottPlot;
using System.Diagnostics;

namespace FrequencyGrapher
{
    internal class Program
    {
        const int xDim = 300, yDim = 200;
        const int freqsPerPlot = 50;

        static void Main(string[] args)
        {
            string pathToData = Path.GetFullPath("../../../../");
            GenerateGraph(pathToData);
        }

        static void GenerateGraph(string pathToData)
        {
            Plot p = new();

            p.Title("Frequency vs Amplitude");

            using (StreamReader reader = new(pathToData+"data.csv"))
            {
                reader.ReadLine(); // Skip header

                string? line = reader.ReadLine();

                double[] freqs = new double[freqsPerPlot], amplitudes = new double[freqsPerPlot];

                while (line != null)
                {
                    for (int i = 0; i < freqsPerPlot; i++)
                    {
                        var data = line.Split(",");
                        freqs[i] = double.Parse(data[0]);
                        amplitudes[i] = double.Parse(data[1]);

                        line = reader.ReadLine();
                        if (line == null)
                            break;
                    }

                    var plt = p.Add.Bars(freqs, amplitudes);
                }
            }

            p.SavePng(pathToData + "Plot.png", xDim, yDim);
        }
    }
}
