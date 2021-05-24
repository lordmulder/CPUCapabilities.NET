/******************************************************************************/
/* CPUCapabilities.NET, by LoRd_MuldeR <MuldeR2@GMX.de>                       */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

using System;
using System.Reflection;

namespace Muldersoft.CPUCapabilitiesDotNet
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                Console.WriteLine("CPUCapabilities v{0:D}.{1:D2} [{2}]", CPU.LibraryVersion.Item1, CPU.LibraryVersion.Item2, BuildDate.ToString("yyyy-MM-dd"));
                Console.WriteLine();
                Console.WriteLine("Architecture: {0}", CPU.Architecture);
                Console.WriteLine("Count: {0}", CPU.Count);
                Console.WriteLine("Vendor: \"{0}\"", OrDefault(CPU.Vendor));
                Console.WriteLine("Information: {0}", CPU.Information);
                Console.WriteLine("Brand: \"{0}\"", OrDefault(CPU.Brand));
                Console.WriteLine("Capabilities: {0}", CPU.Capabilities);
                Console.WriteLine();
                Console.WriteLine("[Debug]");
                Console.WriteLine("IsX64Process: {0}", CPU.IsX64Process);
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

        private static DateTime BuildDate
        {
            get
            {
                Version version = Assembly.GetExecutingAssembly().GetName().Version;
                return new DateTime(2000, 1, 1).Add(new TimeSpan(TimeSpan.TicksPerDay * version.Build + TimeSpan.TicksPerSecond * 2 * version.Revision));
            }
        }
    }
}
