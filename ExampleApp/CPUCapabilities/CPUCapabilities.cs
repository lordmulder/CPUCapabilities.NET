/******************************************************************************/
/* CPUCapabilities.NET, by LoRd_MuldeR <MuldeR2@GMX.de>                       */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

using System;
using System.Runtime.InteropServices;
using System.Text;

namespace Muldersoft.CPUCapabilitiesDotNet
{
    // ==================================================================
    // CPU Flags
    // ==================================================================

    public enum CPUArchitecture : uint
    {
        CPU_ARCH_X86 = 0x00000001,
        CPU_ARCH_X64 = 0x00000002
    }

    [Flags]
    public enum CPUCapabilities : uint
    {
        CPU_3DNOW       = 0x00000001,
        CPU_3DNOWEXT    = 0x00000002,
        CPU_AES         = 0x00000004,
        CPU_AVX         = 0x00000008,
        CPU_AVX2        = 0x00000010,
        CPU_AVX512_BW   = 0x00000020,
        CPU_AVX512_CD   = 0x00000040,
        CPU_AVX512_DQ   = 0x00000080,
        CPU_AVX512_ER   = 0x00000100,
        CPU_AVX512_F    = 0x00000200,
        CPU_AVX512_IFMA = 0x00000400,
        CPU_AVX512_PF   = 0x00000800,
        CPU_AVX512_VL   = 0x00001000,
        CPU_BMI1        = 0x00002000,
        CPU_BMI2        = 0x00004000,
        CPU_FMA3        = 0x00008000,
        CPU_FMA4        = 0x00010000,
        CPU_LZCNT       = 0x00020000,
        CPU_MMX         = 0x00040000,
        CPU_MMXEXT      = 0x00080000,
        CPU_POPCNT      = 0x00100000,
        CPU_RDRND       = 0x00200000,
        CPU_RDSEED      = 0x00400000,
        CPU_SHA         = 0x00800000,
        CPU_SSE         = 0x01000000,
        CPU_SSE2        = 0x02000000,
        CPU_SSE3        = 0x04000000,
        CPU_SSE41       = 0x08000000,
        CPU_SSE42       = 0x10000000,
        CPU_SSE4a       = 0x20000000,
        CPU_SSSE3       = 0x40000000,
        CPU_XOP         = 0x80000000
    }

    // ==================================================================
    // CPUInformation
    // ==================================================================

    public struct CPUInformation
    {
        public CPUInformation(byte type, byte familyExtRaw, byte familyRaw, byte modelExtRaw, byte modelRaw, byte stepping)
        {
            Type         = type;
            FamilyRaw    = familyRaw;
            FamilyExtRaw = familyExtRaw;
            ModelRaw     = modelRaw;
            ModelExtRaw  = modelExtRaw;
            Stepping     = stepping;
        }

        public byte Type         { get; }
        public byte FamilyRaw    { get; }
        public byte FamilyExtRaw { get; }
        public byte ModelRaw     { get; }
        public byte ModelExtRaw  { get; }
        public byte Stepping     { get; }

        public uint Family
        {
            get { return (FamilyRaw == 15U) ? (((uint)FamilyExtRaw) + FamilyRaw) : FamilyRaw; }
        }

        public uint Model
        {
            get { return ((FamilyRaw == 6U) || (FamilyRaw == 15U)) ? ((((uint)ModelExtRaw) << 4) + ModelRaw) : ModelRaw; }
        }

        public override String ToString()
        {
            return String.Format("Type={0}, Family={1}, Model={2}, Stepping={3}", Type, Family, Model, Stepping);
        }
    }

    // ==================================================================
    // CPU class
    // ==================================================================

    public static class CPU
    {
        private static readonly Lazy<bool> m_x64Process = new Lazy<bool>(() => Environment.Is64BitProcess);
        private static readonly Lazy<CPUArchitecture> m_architecture = new Lazy<CPUArchitecture>(GetCPUArchitecture);
        private static readonly Lazy<uint> m_count = new Lazy<uint>(GetCPUCount);
        private static readonly Lazy<CPUInformation> m_information = new Lazy<CPUInformation>(GetCPUInformation);
        private static readonly Lazy<string> m_vendor = new Lazy<string>(GetCPUVendorString);
        private static readonly Lazy<CPUCapabilities> m_capabilities = new Lazy<CPUCapabilities>(GetCPUCapabilities);
        private static readonly Lazy<Tuple<ushort, ushort>> m_libraryVersion = new Lazy<Tuple<ushort, ushort>>(GetCPULibraryVersion);
        private static readonly Lazy<string> m_brand = new Lazy<string>(GetCPUBrandString);

        private static readonly Tuple<ushort, ushort> REQUIRED_LIBRARY_VERSION = Tuple.Create<ushort, ushort>(2, 0);

        // ------------------------------------------------------------------
        // Properties
        // ------------------------------------------------------------------

        public static bool IsX64Process
        {
            get { return m_x64Process.Value; }
        }

        public static CPUArchitecture Architecture
        {
            get { return m_architecture.Value; }
        }

        public static uint Count
        {
            get { return m_count.Value; }
        }

        public static string Vendor
        {
            get { return m_vendor.Value; }
        }

        public static CPUInformation Information
        {
            get { return m_information.Value; }
        }

        public static CPUCapabilities Capabilities
        {
            get { return m_capabilities.Value; }
        }

        public static string Brand
        {
            get { return m_brand.Value; }
        }

        public static Tuple<ushort, ushort> LibraryVersion
        {
            get { return m_libraryVersion.Value; }
        }

        // ------------------------------------------------------------------
        // Initialization methods
        // ------------------------------------------------------------------

        private static CPUArchitecture GetCPUArchitecture()
        {
            try
            {
                VerifyLibraryVersion();
                uint value = IsX64Process ? Internal.GetCPUArchitectureX64() : Internal.GetCPUArchitectureX86();
                return (CPUArchitecture)value;
            }
            catch (Exception e)
            {
                throw new SystemException("Failed to determine CPU architecture!", e);
            }
        }

        private static uint GetCPUCount()
        {
            try
            {
                VerifyLibraryVersion();
                return IsX64Process ? Internal.GetCPUCountX64() : Internal.GetCPUCountX86();
            }
            catch (Exception e)
            {
                throw new SystemException("Failed to determine CPU count!", e);
            }
        }

        private static string GetCPUVendorString()
        {
            const uint BUFFER_SIZE = 13;
            try
            {
                byte[] buffer = new byte[BUFFER_SIZE];
                VerifyLibraryVersion();
                if (IsX64Process ? Internal.GetCPUVendorStringX64(buffer, BUFFER_SIZE) : Internal.GetCPUVendorStringX86(buffer, BUFFER_SIZE))
                {
                    return AllocateAsciiString(buffer);
                }
                return string.Empty;
            }
            catch (Exception e)
            {
                throw new SystemException("Failed to determine CPU vendor string!", e);
            }
        }

        private static CPUInformation GetCPUInformation()
        {
            try
            {
                byte type, familyExt, family, modelExt, model, stepping;
                VerifyLibraryVersion();
                if (IsX64Process ? Internal.GetCPUInformationX64(out type, out familyExt, out family, out modelExt, out model, out stepping) : Internal.GetCPUInformationX86(out type, out familyExt, out family, out modelExt, out model, out stepping))
                {
                    return new CPUInformation(type, familyExt, family, modelExt, model, stepping);
                }
                return new CPUInformation(0, 0, 0, 0, 0, 0);
            }
            catch (Exception e)
            {
                throw new SystemException("Failed to determine CPU family and model!", e);
            }
        }

        private static CPUCapabilities GetCPUCapabilities()
        {
            try
            {
                VerifyLibraryVersion();
                uint value = IsX64Process ? Internal.GetCPUCapabilitiesX64() : Internal.GetCPUCapabilitiesX86();
                return (CPUCapabilities)value;
            }
            catch (Exception e)
            {
                throw new SystemException("Failed to determine CPU capabilities!", e);
            }
        }

        private static string GetCPUBrandString()
        {
            const uint BUFFER_SIZE = 48;
            try
            {
                byte[] buffer = new byte[BUFFER_SIZE];
                VerifyLibraryVersion();
                if (IsX64Process ? Internal.GetCPUBrandStringX64(buffer, BUFFER_SIZE) : Internal.GetCPUBrandStringX86(buffer, BUFFER_SIZE))
                {
                    return AllocateAsciiString(buffer);
                }
                return string.Empty;
            }
            catch (Exception e)
            {
                throw new SystemException("Failed to determine CPU brand string!", e);
            }
        }

        private static Tuple<ushort, ushort> GetCPULibraryVersion()
        {
            try
            {
                uint value = IsX64Process ? Internal.GetCPULibraryVersionX64() : Internal.GetCPULibraryVersionX86();
                return Tuple.Create((ushort)((value >> 16) & 0xFFFF), (ushort)(value & 0xFFFF));
            }
            catch (Exception e)
            {
                throw new SystemException("Failed to determine CPU library version!", e);
            }
        }

        // ------------------------------------------------------------------
        // P/Invoke methods
        // ------------------------------------------------------------------

        private class Internal
        {
            const string DLL_NAME_X86 = "cpu-capabilities-x86.dll";
            const string DLL_NAME_X64 = "cpu-capabilities-x64.dll";

            /* GetCPUArchitecture() */
            [DllImport(DLL_NAME_X64, CallingConvention = CallingConvention.Cdecl)]
            public static extern uint GetCPUArchitectureX64();
            [DllImport(DLL_NAME_X86, CallingConvention = CallingConvention.Cdecl)]
            public static extern uint GetCPUArchitectureX86();

            /* GetCPUCount() */
            [DllImport(DLL_NAME_X64, CallingConvention = CallingConvention.Cdecl)]
            public static extern uint GetCPUCountX64();
            [DllImport(DLL_NAME_X86, CallingConvention = CallingConvention.Cdecl)]
            public static extern uint GetCPUCountX86();

            /* GetCPUVendorString() */
            [DllImport(DLL_NAME_X64, CallingConvention = CallingConvention.Cdecl)]
            public static extern bool GetCPUVendorStringX64([Out] byte[] buffer, uint size);
            [DllImport(DLL_NAME_X86, CallingConvention = CallingConvention.Cdecl)]
            public static extern bool GetCPUVendorStringX86([Out] byte[] buffer, uint size);

            /* GetCPUInformation() */
            [DllImport(DLL_NAME_X64, CallingConvention = CallingConvention.Cdecl)]
            public static extern bool GetCPUInformationX64(out byte type, out byte familyExt, out byte family, out byte modelExt, out byte model, out byte stepping);
            [DllImport(DLL_NAME_X86, CallingConvention = CallingConvention.Cdecl)]
            public static extern bool GetCPUInformationX86(out byte type, out byte familyExt, out byte family, out byte modelExt, out byte model, out byte stepping);

            /* GetCPUCapabilities() */
            [DllImport(DLL_NAME_X64, CallingConvention = CallingConvention.Cdecl)]
            public static extern uint GetCPUCapabilitiesX64();
            [DllImport(DLL_NAME_X86, CallingConvention = CallingConvention.Cdecl)]
            public static extern uint GetCPUCapabilitiesX86();

            /* GetCPUBrandString() */
            [DllImport(DLL_NAME_X64, CallingConvention = CallingConvention.Cdecl)]
            public static extern bool GetCPUBrandStringX64([Out] byte[] buffer, uint size);
            [DllImport(DLL_NAME_X86, CallingConvention = CallingConvention.Cdecl)]
            public static extern bool GetCPUBrandStringX86([Out] byte[] buffer, uint size);

            /* GetCPULibraryVersion() */
            [DllImport(DLL_NAME_X64, CallingConvention = CallingConvention.Cdecl)]
            public static extern uint GetCPULibraryVersionX64();
            [DllImport(DLL_NAME_X86, CallingConvention = CallingConvention.Cdecl)]
            public static extern uint GetCPULibraryVersionX86();
        }

        // ------------------------------------------------------------------
        // Utility methods
        // ------------------------------------------------------------------

        private static void VerifyLibraryVersion()
        {
            if ((LibraryVersion.Item1 != REQUIRED_LIBRARY_VERSION.Item1) || (LibraryVersion.Item2 < REQUIRED_LIBRARY_VERSION.Item2))
            {
                throw new InvalidOperationException(string.Format("CPU library version {0} is not supported! (required version: {1})", m_libraryVersion.Value.Item1, REQUIRED_LIBRARY_VERSION));
            }
        }

        private static string AllocateAsciiString(byte[] buffer)
        {
            int index = Array.IndexOf(buffer, (byte)0);
            if (index >= 0)
            {
                return (index > 0) ? Encoding.ASCII.GetString(buffer, 0, index) : string.Empty;
            }
            return Encoding.ASCII.GetString(buffer);
        }
    }
}
