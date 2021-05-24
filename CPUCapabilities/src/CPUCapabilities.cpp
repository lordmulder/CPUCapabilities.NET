/******************************************************************************/
/* CPUCapabilities.NET, by LoRd_MuldeR <MuldeR2@GMX.de>                       */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#define WIN32_LEAN_AND_MEAN 1

#include <Windows.h>
#include <intrin.h>

#pragma intrinsic(__cpuid)
#pragma intrinsic(_xgetbv)

static const WORD LIBRARY_VERSION_MAJOR = 2U;
static const WORD LIBRARY_VERSION_MINOR = 0U;

#if ((!defined(_M_X64)) || (_M_X64 < 100)) && ((!defined(_M_IX86)) || (_M_IX86 < 600))
#error Unsupported CPU architecture!
#endif

// ==========================================================================
// CPU FLAGS
// ==========================================================================

#define CPU_ARCH_X86 0x00000001
#define CPU_ARCH_X64 0x00000002

#define CPU_CAPABILITY_3DNOW            0x0000000000000001
#define CPU_CAPABILITY_3DNOWEXT         0x0000000000000002
#define CPU_CAPABILITY_ADX              0x0000000000000004
#define CPU_CAPABILITY_AES              0x0000000000000008
#define CPU_CAPABILITY_AVX              0x0000000000000010
#define CPU_CAPABILITY_AVX2             0x0000000000000020
#define CPU_CAPABILITY_AVX512_BITALG    0x0000000000000040
#define CPU_CAPABILITY_AVX512_BW        0x0000000000000080
#define CPU_CAPABILITY_AVX512_CD        0x0000000000000100
#define CPU_CAPABILITY_AVX512_DQ        0x0000000000000200
#define CPU_CAPABILITY_AVX512_ER        0x0000000000000400
#define CPU_CAPABILITY_AVX512_F         0x0000000000000800
#define CPU_CAPABILITY_AVX512_IFMA      0x0000000000001000
#define CPU_CAPABILITY_AVX512_PF        0x0000000000002000
#define CPU_CAPABILITY_AVX512_VBMI      0x0000000000004000
#define CPU_CAPABILITY_AVX512_VBMI2     0x0000000000008000
#define CPU_CAPABILITY_AVX512_VL        0x0000000000010000
#define CPU_CAPABILITY_AVX512_VNNI      0x0000000000020000
#define CPU_CAPABILITY_AVX512_VPOPCNTDQ 0x0000000000040000
#define CPU_CAPABILITY_BMI1             0x0000000000080000
#define CPU_CAPABILITY_BMI2             0x0000000000100000
#define CPU_CAPABILITY_CLSFH            0x0000000000200000
#define CPU_CAPABILITY_CMOV             0x0000000000400000
#define CPU_CAPABILITY_CX16             0x0000000000800000
#define CPU_CAPABILITY_CX8              0x0000000001000000
#define CPU_CAPABILITY_ERMS             0x0000000002000000
#define CPU_CAPABILITY_FMA3             0x0000000004000000
#define CPU_CAPABILITY_FMA4             0x0000000008000000
#define CPU_CAPABILITY_FPU              0x0000000010000000
#define CPU_CAPABILITY_FSGSBASE         0x0000000020000000
#define CPU_CAPABILITY_FXSR             0x0000000040000000
#define CPU_CAPABILITY_HLE              0x0000000080000000
#define CPU_CAPABILITY_HTT              0x0000000100000000
#define CPU_CAPABILITY_INVPCID          0x0000000200000000
#define CPU_CAPABILITY_LAHF             0x0000000400000000
#define CPU_CAPABILITY_LZCNT            0x0000000800000000
#define CPU_CAPABILITY_MMX              0x0000001000000000
#define CPU_CAPABILITY_MMXEXT           0x0000002000000000
#define CPU_CAPABILITY_MONITOR          0x0000004000000000
#define CPU_CAPABILITY_MOVBE            0x0000008000000000
#define CPU_CAPABILITY_MSR              0x0000010000000000
#define CPU_CAPABILITY_OSXSAVE          0x0000020000000000
#define CPU_CAPABILITY_PCLMULQDQ        0x0000040000000000
#define CPU_CAPABILITY_POPCNT           0x0000080000000000
#define CPU_CAPABILITY_PREFETCHWT1      0x0000100000000000
#define CPU_CAPABILITY_RDRND            0x0000200000000000
#define CPU_CAPABILITY_RDSEED           0x0000400000000000
#define CPU_CAPABILITY_RDTSCP           0x0000800000000000
#define CPU_CAPABILITY_RTM              0x0001000000000000
#define CPU_CAPABILITY_SEP              0x0002000000000000
#define CPU_CAPABILITY_SHA              0x0004000000000000
#define CPU_CAPABILITY_SSE              0x0008000000000000
#define CPU_CAPABILITY_SSE2             0x0010000000000000
#define CPU_CAPABILITY_SSE3             0x0020000000000000
#define CPU_CAPABILITY_SSE41            0x0040000000000000
#define CPU_CAPABILITY_SSE42            0x0080000000000000
#define CPU_CAPABILITY_SSE4a            0x0100000000000000
#define CPU_CAPABILITY_SSSE3            0x0200000000000000
#define CPU_CAPABILITY_SYSCALL          0x0400000000000000
#define CPU_CAPABILITY_TBM              0x0800000000000000
#define CPU_CAPABILITY_TSC              0x1000000000000000
#define CPU_CAPABILITY_XOP              0x2000000000000000
#define CPU_CAPABILITY_XSAVE            0x4000000000000000

// ==========================================================================
// Utility Functions
// ==========================================================================

static void copy_string(char *const dst, const char* const src, const DWORD count)
{
	DWORD pos;
	for (pos = 0U; pos < count; ++pos)
	{
		dst[pos] = src[pos];
	}
}

static void cpuid(const DWORD idx, DWORD &eax, DWORD &ebx, DWORD &ecx, DWORD &edx)
{
	DWORD cpu_info[4U];
	__cpuid(reinterpret_cast<int*>(cpu_info), *reinterpret_cast<const int*>(&idx));
	eax = cpu_info[0U], ebx = cpu_info[1U], ecx = cpu_info[2U], edx = cpu_info[3U];
}

// ==========================================================================
// Detect CPU Architecture
// ==========================================================================

typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);

static DWORD get_cpu_architecture(void)
{
#if !defined(_M_X64)
	const HMODULE kernel32 = GetModuleHandleW(L"kernel32");
	if (NULL != kernel32)
	{
		const LPFN_ISWOW64PROCESS is_wow64_process = (LPFN_ISWOW64PROCESS)GetProcAddress(kernel32, "IsWow64Process");
		if (NULL != is_wow64_process)
		{
			BOOL is_wow64 = FALSE;
			if (is_wow64_process(GetCurrentProcess(), &is_wow64))
			{
				return is_wow64 ? CPU_ARCH_X64 : CPU_ARCH_X86;
			}
		}
	}
	return CPU_ARCH_X86;
#else
	return CPU_ARCH_X64;
#endif
}

// ==========================================================================
// Detect CPU Count
// ==========================================================================

static DWORD get_cpu_count(void)
{
	DWORD_PTR process_mask, system_mask;
	DWORD count = 0U;
	if (GetProcessAffinityMask(GetCurrentProcess(), &process_mask, &system_mask))
	{
		while (system_mask)
		{
			if ((system_mask & 1U) == 1U)
			{
				++count;
			}
			system_mask >>= 1;
		}
	}
	return max(count, 1U);
}

// ==========================================================================
// Detect CPU Vendor
// ==========================================================================

static BOOL get_cpu_vendor_string(char *const buffer, const DWORD size)
{
	DWORD eax, ebx, ecx, edx;
	
	if (size < 13U)
	{
		if (size != 0U)
		{
			buffer[0U] = '\0';
		}
		return FALSE;
	}

	cpuid(0U, eax, ebx, ecx, edx);

	copy_string(buffer + 0U, reinterpret_cast<const char*>(&ebx), 4U);
	copy_string(buffer + 4U, reinterpret_cast<const char*>(&edx), 4U);
	copy_string(buffer + 8U, reinterpret_cast<const char*>(&ecx), 4U);

	buffer[12U] = '\0';
	return TRUE;
}

// ==========================================================================
// Detect CPU Family & Model
// ==========================================================================

static BOOL get_cpu_information(BYTE *const type, BYTE *const family_ext, BYTE *const family, BYTE *const model_ext, BYTE *const model, BYTE *const stepping)
{
	DWORD eax, ebx, ecx, edx;
	
	cpuid(0U, eax, ebx, ecx, edx);
	if (eax < 1U)
	{
		*family = *model = 0U;
		return FALSE;
	}

	cpuid(1U, eax, ebx, ecx, edx);
	*stepping   = (BYTE)((eax >>  0) & 0x0F);
	*model      = (BYTE)((eax >>  4) & 0x0F);
	*family     = (BYTE)((eax >>  8) & 0x0F);
	*model_ext  = (BYTE)((eax >> 16) & 0x0F);
	*type       = (BYTE)((eax >> 12) & 0x03);
	*family_ext = (BYTE)((eax >> 20) & 0xFF);

	return TRUE;
}

// ==========================================================================
// Detect CPU Capabilities
// ==========================================================================

static UINT64 get_cpu_capabilities(void)
{
	DWORD eax, ebx, ecx, edx, max_basic_caps;
	UINT64 cpu_caps = 0U;

	cpuid(0U, max_basic_caps, ebx, ecx, edx);
	if (max_basic_caps < 1U)
	{
		goto finish;
	}

	cpuid(1U, eax, ebx, ecx, edx);
	if (edx & 0x00000001)
	{
		cpu_caps |= CPU_CAPABILITY_FPU;
	}
	if (edx & 0x00000010)
	{
		cpu_caps |= CPU_CAPABILITY_TSC;
	}
	if (edx & 0x00000020)
	{
		cpu_caps |= CPU_CAPABILITY_MSR;
	}
	if (edx & 0x00000100)
	{
		cpu_caps |= CPU_CAPABILITY_CX8;
	}
	if (edx & 0x00000800)
	{
		cpu_caps |= CPU_CAPABILITY_SEP;
	}
	if (edx & 0x00008000)
	{
		cpu_caps |= CPU_CAPABILITY_CMOV;
	}
	if (edx & 0x00800000)
	{
		cpu_caps |= CPU_CAPABILITY_MMX;
	}
	else
	{
		goto finish;
	}

	if (edx & 0x00080000)
	{
		cpu_caps |= CPU_CAPABILITY_CLSFH;
	}
	if (edx & 0x01000000)
	{
		cpu_caps |= CPU_CAPABILITY_FXSR;
	}
	if (edx & 0x02000000)
	{
		cpu_caps |= CPU_CAPABILITY_SSE | CPU_CAPABILITY_MMXEXT;
	}
	if (edx & 0x04000000)
	{
		cpu_caps |= CPU_CAPABILITY_SSE2;
	}
	if (edx & 0x10000000)
	{
		cpu_caps |= CPU_CAPABILITY_HTT;
	}

	if (ecx & 0x00000001)
	{
		cpu_caps |= CPU_CAPABILITY_SSE3;
	}
	if (ecx & 0x00000002)
	{
		cpu_caps |= CPU_CAPABILITY_PCLMULQDQ;
	}
	if (ecx & 0x00000008)
	{
		cpu_caps |= CPU_CAPABILITY_MONITOR;
	}
	if (ecx & 0x00000200)
	{
		cpu_caps |= CPU_CAPABILITY_SSSE3;
	}
	if (ecx & 0x00002000)
	{
		cpu_caps |= CPU_CAPABILITY_CX16;
	}
	if (ecx & 0x00080000)
	{
		cpu_caps |= CPU_CAPABILITY_SSE41;
	}
	if (ecx & 0x00100000)
	{
		cpu_caps |= CPU_CAPABILITY_SSE42;
	}
	if (ecx & 0x00400000)
	{
		cpu_caps |= CPU_CAPABILITY_MOVBE;
	}
	if (ecx & 0x00800000)
	{
		cpu_caps |= CPU_CAPABILITY_POPCNT;
	}
	if (ecx & 0x02000000)
	{
		cpu_caps |= CPU_CAPABILITY_AES;
	}
	if (ecx & 0x04000000)
	{
		cpu_caps |= CPU_CAPABILITY_XSAVE;
	}
	if (ecx & 0x08000000)
	{
		cpu_caps |= CPU_CAPABILITY_OSXSAVE;
	}
	if (ecx & 0x40000000)
	{
		cpu_caps |= CPU_CAPABILITY_RDRND;
	}

	if ((cpu_caps & CPU_CAPABILITY_OSXSAVE) == CPU_CAPABILITY_OSXSAVE)
	{
		UINT64 xcr0 = _xgetbv(0U);
		if ((xcr0 & 0x6) == 0x6)
		{
			if (ecx & 0x00001000)
			{
				cpu_caps |= CPU_CAPABILITY_FMA3;
			}
			if (ecx & 0x10000000)
			{
				cpu_caps |= CPU_CAPABILITY_AVX;
			}
			if (max_basic_caps >= 7U)
			{
				cpuid(7U, eax, ebx, ecx, edx);
				if (ebx & 0x00000001)
				{
					cpu_caps |= CPU_CAPABILITY_FSGSBASE;
				}
				if (ebx & 0x00000008)
				{
					cpu_caps |= CPU_CAPABILITY_BMI1;
				}
				if (ebx & 0x00000010)
				{
					cpu_caps |= CPU_CAPABILITY_HLE;
				}
				if (ebx & 0x00000020)
				{
					cpu_caps |= CPU_CAPABILITY_AVX2;
				}
				if (ebx & 0x00000100)
				{
					cpu_caps |= CPU_CAPABILITY_BMI2;
				}
				if (ebx & 0x00000200)
				{
					cpu_caps |= CPU_CAPABILITY_ERMS;
				}
				if (ebx & 0x00000400)
				{
					cpu_caps |= CPU_CAPABILITY_INVPCID;
				}
				if (ebx & 0x00000800)
				{
					cpu_caps |= CPU_CAPABILITY_RTM;
				}
				if (ebx & 0x00040000)
				{
					cpu_caps |= CPU_CAPABILITY_RDSEED;
				}
				if (ebx & 0x00080000)
				{
					cpu_caps |= CPU_CAPABILITY_ADX;
				}
				if (ebx & 0x20000000)
				{
					cpu_caps |= CPU_CAPABILITY_SHA;
				}
				if (ecx & 0x00000001)
				{
					cpu_caps |= CPU_CAPABILITY_PREFETCHWT1;
				}
				if ((xcr0 & 0xE0) == 0xE0)
				{
					if (ebx & 0x00010000)
					{
						cpu_caps |= CPU_CAPABILITY_AVX512_F;
					}
					if (ebx & 0x00020000)
					{
						cpu_caps |= CPU_CAPABILITY_AVX512_DQ;
					}
					if (ebx & 0x00040000)
					{
						cpu_caps |= CPU_CAPABILITY_AVX512_IFMA;
					}
					if (ebx & 0x04000000)
					{
						cpu_caps |= CPU_CAPABILITY_AVX512_PF;
					}
					if (ebx & 0x08000000)
					{
						cpu_caps |= CPU_CAPABILITY_AVX512_ER;
					}
					if (ebx & 0x10000000)
					{
						cpu_caps |= CPU_CAPABILITY_AVX512_CD;
					}
					if (ebx & 0x40000000)
					{
						cpu_caps |= CPU_CAPABILITY_AVX512_BW;
					}
					if (ebx & 0x80000000)
					{
						cpu_caps |= CPU_CAPABILITY_AVX512_VL;
					}
					if (ecx & 0x00000002)
					{
						cpu_caps |= CPU_CAPABILITY_AVX512_VBMI;
					}
					if (ecx & 0x00000040)
					{
						cpu_caps |= CPU_CAPABILITY_AVX512_VBMI2;
					}
					if (ecx & 0x00000800)
					{
						cpu_caps |= CPU_CAPABILITY_AVX512_VNNI;
					}
					if (ecx & 0x00001000)
					{
						cpu_caps |= CPU_CAPABILITY_AVX512_BITALG;
					}
					if (ecx & 0x00004000)
					{
						cpu_caps |= CPU_CAPABILITY_AVX512_VPOPCNTDQ;
					}
				}
			}
		}
	}

	cpuid(0x80000000, eax, ebx, ecx, edx);
	if (eax >= 0x80000001)
	{
		cpuid(0x80000001, eax, ebx, ecx, edx);
		if (edx & 0x00000001)
		{
			cpu_caps |= CPU_CAPABILITY_FPU;
		}
		if (edx & 0x00000010)
		{
			cpu_caps |= CPU_CAPABILITY_TSC;
		}
		if (edx & 0x00000020)
		{
			cpu_caps |= CPU_CAPABILITY_MSR;
		}
		if (edx & 0x00000800)
		{
			cpu_caps |= CPU_CAPABILITY_SYSCALL;
		}
		if (edx & 0x00008000)
		{
			cpu_caps |= CPU_CAPABILITY_CMOV;
		}
		if (edx & 0x00400000)
		{
			cpu_caps |= CPU_CAPABILITY_MMXEXT;
		}
		if (edx & 0x00800000)
		{
			cpu_caps |= CPU_CAPABILITY_MMX;
		}
		if (edx & 0x01000000)
		{
			cpu_caps |= CPU_CAPABILITY_FXSR;
		}
		if (edx & 0x08000000)
		{
			cpu_caps |= CPU_CAPABILITY_RDTSCP;
		}
		if (edx & 0x40000000)
		{
			cpu_caps |= CPU_CAPABILITY_3DNOWEXT;
		}
		if (edx & 0x80000000)
		{
			cpu_caps |= CPU_CAPABILITY_3DNOW;
		}
		if (ecx & 0x00000001)
		{
			cpu_caps |= CPU_CAPABILITY_LAHF;
		}
		if (ecx & 0x00000020)
		{
			cpu_caps |= CPU_CAPABILITY_LZCNT;
		}
		if (ecx & 0x00000040)
		{
			cpu_caps |= CPU_CAPABILITY_SSE4a;
		}
		if (cpu_caps & CPU_CAPABILITY_AVX)
		{
			if (ecx & 0x00000800)
			{
				cpu_caps |= CPU_CAPABILITY_XOP;
			}
			if (ecx & 0x00010000)
			{
				cpu_caps |= CPU_CAPABILITY_FMA4;
			}
		}
		if (ecx & 0x00200000)
		{
			cpu_caps |= CPU_CAPABILITY_TBM;
		}
	}

finish:
	return cpu_caps;
}

// ==========================================================================
// Detect CPU Vendor
// ==========================================================================

static BOOL get_cpu_brand_string(char *const buffer, const DWORD size)
{
	DWORD index, offset;
	DWORD eax, ebx, ecx, edx;

	if (size < 48U)
	{
		if (size != 0U)
		{
			buffer[0U] = '\0';
		}
		return FALSE;
	}

	cpuid(0U, eax, ebx, ecx, edx);
	if (eax < 1U)
	{
		buffer[0U] = '\0';
		return FALSE;
	}

	cpuid(0x80000000, eax, ebx, ecx, edx);
	if (eax >= 0x80000004)
	{
		for (index = 0U, offset = 0U; index < 3U; ++index, offset += 16U)
		{
			cpuid(0x80000002 + index, eax, ebx, ecx, edx);
			copy_string(buffer + offset + 0x0, reinterpret_cast<const char*>(&eax), 4U);
			copy_string(buffer + offset + 0x4, reinterpret_cast<const char*>(&ebx), 4U);
			copy_string(buffer + offset + 0x8, reinterpret_cast<const char*>(&ecx), 4U);
			copy_string(buffer + offset + 0xC, reinterpret_cast<const char*>(&edx), 4U);
		}
		buffer[47U] = '\0';
		return TRUE;
	}

	buffer[0U] = '\0';
	return FALSE;
}

// ==========================================================================
// DLL Exports
// ==========================================================================

#ifndef _M_X64
#define DLL_ENTRY(ENTRY) ENTRY##X86
#else
#define DLL_ENTRY(ENTRY) ENTRY##X64
#endif

extern "C"
{
	__declspec(dllexport) DWORD DLL_ENTRY(GetCPULibraryVersion)(void)
	{
		return (((DWORD)LIBRARY_VERSION_MAJOR) << 16U) | ((DWORD)LIBRARY_VERSION_MINOR);
	}

	__declspec(dllexport) DWORD DLL_ENTRY(GetCPUArchitecture)(void)
	{
		return get_cpu_architecture();
	}

	__declspec(dllexport) DWORD DLL_ENTRY(GetCPUCount)(void)
	{
		return get_cpu_count();
	}

	__declspec(dllexport) BOOL DLL_ENTRY(GetCPUVendorString)(char *const buffer, const DWORD size)
	{
		return get_cpu_vendor_string(buffer, size);
	}

	__declspec(dllexport) BOOL DLL_ENTRY(GetCPUInformation)(BYTE *const type, BYTE *const family_ext, BYTE *const family, BYTE *const model_ext, BYTE *const model, BYTE *const stepping)
	{
		return get_cpu_information(type, family_ext, family, model_ext, model, stepping);
	}

	__declspec(dllexport) UINT64 DLL_ENTRY(GetCPUCapabilities)(void)
	{
		return get_cpu_capabilities();
	}

	__declspec(dllexport) BOOL DLL_ENTRY(GetCPUBrandString)(char *const buffer, const DWORD size)
	{
		return get_cpu_brand_string(buffer, size);
	}
}
