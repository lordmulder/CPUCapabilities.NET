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
    public enum CPUCapabilities : ulong
    {
        CPU_3DNOW            = 0x0000000000000001,
        CPU_3DNOWEXT         = 0x0000000000000002,
        CPU_ADX              = 0x0000000000000004,
        CPU_AES              = 0x0000000000000008,
        CPU_AVX              = 0x0000000000000010,
        CPU_AVX2             = 0x0000000000000020,
        CPU_AVX512_BITALG    = 0x0000000000000040,
        CPU_AVX512_BW        = 0x0000000000000080,
        CPU_AVX512_CD        = 0x0000000000000100,
        CPU_AVX512_DQ        = 0x0000000000000200,
        CPU_AVX512_ER        = 0x0000000000000400,
        CPU_AVX512_F         = 0x0000000000000800,
        CPU_AVX512_IFMA      = 0x0000000000001000,
        CPU_AVX512_PF        = 0x0000000000002000,
        CPU_AVX512_VBMI      = 0x0000000000004000,
        CPU_AVX512_VBMI2     = 0x0000000000008000,
        CPU_AVX512_VL        = 0x0000000000010000,
        CPU_AVX512_VNNI      = 0x0000000000020000,
        CPU_AVX512_VPOPCNTDQ = 0x0000000000040000,
        CPU_BMI1             = 0x0000000000080000,
        CPU_BMI2             = 0x0000000000100000,
        CPU_CLFSH            = 0x0000000000200000,
        CPU_CMOV             = 0x0000000000400000,
        CPU_CX16             = 0x0000000000800000,
        CPU_CX8              = 0x0000000001000000,
        CPU_ERMS             = 0x0000000002000000,
        CPU_FMA3             = 0x0000000004000000,
        CPU_FMA4             = 0x0000000008000000,
        CPU_FPU              = 0x0000000010000000,
        CPU_FSGSBASE         = 0x0000000020000000,
        CPU_FXSR             = 0x0000000040000000,
        CPU_HLE              = 0x0000000080000000,
        CPU_HTT              = 0x0000000100000000,
        CPU_INVPCID          = 0x0000000200000000,
        CPU_LAHF             = 0x0000000400000000,
        CPU_LZCNT            = 0x0000000800000000,
        CPU_MMX              = 0x0000001000000000,
        CPU_MMXEXT           = 0x0000002000000000,
        CPU_MONITOR          = 0x0000004000000000,
        CPU_MOVBE            = 0x0000008000000000,
        CPU_MSR              = 0x0000010000000000,
        CPU_OSXSAVE          = 0x0000020000000000,
        CPU_PCLMULQDQ        = 0x0000040000000000,
        CPU_POPCNT           = 0x0000080000000000,
        CPU_PREFETCHWT1      = 0x0000100000000000,
        CPU_RDRND            = 0x0000200000000000,
        CPU_RDSEED           = 0x0000400000000000,
        CPU_RDTSCP           = 0x0000800000000000,
        CPU_RTM              = 0x0001000000000000,
        CPU_SEP              = 0x0002000000000000,
        CPU_SHA              = 0x0004000000000000,
        CPU_SSE              = 0x0008000000000000,
        CPU_SSE2             = 0x0010000000000000,
        CPU_SSE3             = 0x0020000000000000,
        CPU_SSE41            = 0x0040000000000000,
        CPU_SSE42            = 0x0080000000000000,
        CPU_SSE4a            = 0x0100000000000000,
        CPU_SSSE3            = 0x0200000000000000,
        CPU_SYSCALL          = 0x0400000000000000,
        CPU_TBM              = 0x0800000000000000,
        CPU_TSC              = 0x1000000000000000,
        CPU_XOP              = 0x2000000000000000,
        CPU_XSAVE            = 0x4000000000000000
    }

    // ==================================================================
    // CPUInformation
    // ==================================================================

    public struct CPUInformation
    {
        public CPUInformation(byte type, byte familyExt, byte family, byte modelExt, byte model, byte stepping)
        {
            Type         = type;
            RawFamily    = family;
            RawFamilyExt = familyExt;
            RawModel     = model;
            RawModelExt  = modelExt;
            Stepping     = stepping;
        }

        public byte Type         { get; }
        public byte RawFamily    { get; }
        public byte RawFamilyExt { get; }
        public byte RawModel     { get; }
        public byte RawModelExt  { get; }
        public byte Stepping     { get; }

        public uint Family
        {
            get { return (RawFamily == 15U) ? (((uint)RawFamilyExt) + RawFamily) : RawFamily; }
        }

        public uint Model
        {
            get { return ((RawFamily == 6U) || (RawFamily == 15U)) ? ((((uint)RawModelExt) << 4) + RawModel) : RawModel; }
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
                ulong value = IsX64Process ? Internal.GetCPUCapabilitiesX64() : Internal.GetCPUCapabilitiesX86();
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
            public static extern ulong GetCPUCapabilitiesX64();
            [DllImport(DLL_NAME_X86, CallingConvention = CallingConvention.Cdecl)]
            public static extern ulong GetCPUCapabilitiesX86();

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
