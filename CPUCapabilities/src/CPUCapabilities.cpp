/******************************************************************************/
/* CPUCapabilities.NET, by LoRd_MuldeR <MuldeR2@GMX.de>                       */
/* This work has been released under the CC0 1.0 Universal license!           */
/******************************************************************************/

#define WIN32_LEAN_AND_MEAN 1

#include <Windows.h>
#include <intrin.h>

#pragma intrinsic(__cpuid)
#pragma intrinsic(_xgetbv)

static const WORD LIBRARY_VERSION_MAJOR = 1U;
static const WORD LIBRARY_VERSION_MINOR = 0U;

// ==========================================================================
// CPU Capabilities
// ==========================================================================

#define CPU_CAPABILITY_MMX    0x00000001
#define CPU_CAPABILITY_SSE    0x00000002
#define CPU_CAPABILITY_SSE2   0x00000004
#define CPU_CAPABILITY_LZCNT  0x00000008
#define CPU_CAPABILITY_SSE3   0x00000010
#define CPU_CAPABILITY_SSSE3  0x00000020
#define CPU_CAPABILITY_SSE4   0x00000040
#define CPU_CAPABILITY_SSE42  0x00000080
#define CPU_CAPABILITY_AVX    0x00000100
#define CPU_CAPABILITY_XOP    0x00000200
#define CPU_CAPABILITY_FMA4   0x00000400
#define CPU_CAPABILITY_FMA3   0x00000800
#define CPU_CAPABILITY_BMI1   0x00001000
#define CPU_CAPABILITY_BMI2   0x00002000
#define CPU_CAPABILITY_AVX2   0x00004000
#define CPU_CAPABILITY_AVX512 0x00008000

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
// Detect CPU Count
// ==========================================================================

static DWORD get_cpu_count(void)
{
	DWORD_PTR process_mask, system_mask, current_bit;
	DWORD count = 0U;
	if (GetProcessAffinityMask(GetCurrentProcess(), &process_mask, &system_mask))
	{
		for (current_bit = 1U; current_bit != 0U; current_bit <<= 1)
		{
			if (system_mask & current_bit)
			{
				++count;
			}
		}
	}
	return count;
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

static BOOL get_cpu_family_and_model(DWORD *const family, DWORD *const model, DWORD *const stepping)
{
	DWORD eax, ebx, ecx, edx;
	
	cpuid(0U, eax, ebx, ecx, edx);
	if (eax < 1U)
	{
		*family = *model = 0U;
		return FALSE;
	}

	cpuid(1U, eax, ebx, ecx, edx);
	*family   = ((eax >> 8) & 0xF) + ((eax >> 20) & 0xFF);
	*model    = ((eax >> 4) & 0xF) + ((eax >> 12) & 0xF0);
	*stepping = eax & 0xF;

	return TRUE;
}

// ==========================================================================
// Detect CPU Capabilities
// ==========================================================================

static DWORD get_cpu_capabilities(void)
{
	DWORD cpu_caps = 0U, max_basic_caps = 0U;
	DWORD eax, ebx, ecx, edx;

	cpuid(0U, max_basic_caps, ebx, ecx, edx);
	if (max_basic_caps < 1U)
	{
		goto finish;
	}

	cpuid(1U, eax, ebx, ecx, edx);
	if (edx & 0x00800000)
	{
		cpu_caps |= CPU_CAPABILITY_MMX;
	}
	else
	{
		goto finish;
	}

	if (edx & 0x02000000)
	{
		cpu_caps |= CPU_CAPABILITY_SSE;
	}
	if (edx & 0x04000000)
	{
		cpu_caps |= CPU_CAPABILITY_SSE2;
	}
	if (ecx & 0x00000001)
	{
		cpu_caps |= CPU_CAPABILITY_SSE3;
	}
	if (ecx & 0x00000200)
	{
		cpu_caps |= CPU_CAPABILITY_SSSE3;
	}
	if (ecx & 0x00080000)
	{
		cpu_caps |= CPU_CAPABILITY_SSE4;
	}
	if (ecx & 0x00100000)
	{
		cpu_caps |= CPU_CAPABILITY_SSE42;
	}

	if (ecx & 0x08000000)
	{
		UINT64 xcr0 = _xgetbv(0U);
		if ((xcr0 & 0x6) == 0x6)
		{
			if (ecx & 0x10000000)
			{
				cpu_caps |= CPU_CAPABILITY_AVX;
			}
			if (ecx & 0x00001000)
			{
				cpu_caps |= CPU_CAPABILITY_FMA3;
			}
			if (max_basic_caps >= 7U)
			{
				cpuid(7U, eax, ebx, ecx, edx);
				if (ebx & 0x00000008)
				{
					cpu_caps |= CPU_CAPABILITY_BMI1;
				}
				if (ebx & 0x00000100)
				{
					cpu_caps |= CPU_CAPABILITY_BMI2;
				}
				if (ebx & 0x00000020)
				{
					cpu_caps |= CPU_CAPABILITY_AVX2;
				}
				if (((xcr0 & 0xE0) == 0xE0) && ((ebx & 0xD0030000) == 0xD0030000))
				{
					cpu_caps |= CPU_CAPABILITY_AVX512;
				}
			}
		}
	}

	cpuid(0x80000000, eax, ebx, ecx, edx);
	if (eax >= 0x80000001)
	{
		cpuid(0x80000001, eax, ebx, ecx, edx);
		if (ecx & 0x00000020)
		{
			cpu_caps |= CPU_CAPABILITY_LZCNT;
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

	__declspec(dllexport) DWORD DLL_ENTRY(GetCPUCount)(void)
	{
		return get_cpu_count();
	}

	__declspec(dllexport) BOOL DLL_ENTRY(GetCPUVendorString)(char *const buffer, const DWORD size)
	{
		return get_cpu_vendor_string(buffer, size);
	}

	__declspec(dllexport) BOOL DLL_ENTRY(GetCPUFamilyAndModel)(DWORD *const family, DWORD *const model, DWORD *const stepping)
	{
		return get_cpu_family_and_model(family, model, stepping);
	}

	__declspec(dllexport) DWORD DLL_ENTRY(GetCPUCapabilities)(void)
	{
		return get_cpu_capabilities();
	}

	__declspec(dllexport) BOOL DLL_ENTRY(GetCPUBrandString)(char *const buffer, const DWORD size)
	{
		return get_cpu_brand_string(buffer, size);
	}
}
