/******************************************************************************/
/* CPUCapabilities.NET, by LoRd_MuldeR <MuldeR2@GMX.de>                       */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

using System;

namespace Muldersoft.CPUCapabilitiesDotNet
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                Console.WriteLine("[CPUCapabilities]");
                Console.WriteLine("Architecture: {0}", CPUCapabilities.Architecture);
                Console.WriteLine("Count: {0}", CPUCapabilities.Count);
                Console.WriteLine("VendorString: \"{0}\"", OrDefault(CPUCapabilities.VendorString));
                Console.WriteLine("BrandString: \"{0}\"", OrDefault(CPUCapabilities.BrandString));
                Console.WriteLine("FamilyAndModel: Family={0}, Model={1}, Stepping={2}", CPUCapabilities.FamilyAndModel.Item1, CPUCapabilities.FamilyAndModel.Item2, CPUCapabilities.FamilyAndModel.Item3);
                Console.WriteLine("Capabilities: {0}", CPUCapabilities.Capabilities);
                Console.WriteLine();
                Console.WriteLine("[Debug]");
                Console.WriteLine("IsX64Process: {0}", CPUCapabilities.IsX64Process);
                Console.WriteLine();
            }
            catch (Exception e)
            {
                Console.Error.WriteLine();
                Console.Error.WriteLine("Error: Something went seriously wrong !!!");
                Console.Error.WriteLine();
                Console.Error.WriteLine(e.ToString());
            }
        }

        private static string OrDefault(string text)
        {
            return string.IsNullOrEmpty(text) ? "N/A" : text;
        }
    }
}
