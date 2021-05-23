---
title: "![CPUCapabilities.NET](etc/img/Logo.png)"
---


Introduction
============

**CPUCapabilities.NET** provides detailed information about the underlying CPU to applications running on the [Microsoft .NET Framework](https://en.wikipedia.org/wiki/.NET_Framework). It consists of two components: A tiny "unmanaged" helper library (written in C++), which performs the actual CPU detection, and a "managed" wrapper class (written in C#), which encapsulates the required P/Invoke calls.

The information provided by *CPUCapabilities.NET* include the CPU architecture, the number of (logical) CPU cores, the CPU "vendor" and "brand" strings, the CPU "model" and "family" IDs as well as the supported CPU capabilities (instruction set extensions), such as MMX, 3DNow!, SSE, SSE2, SSE3, SSSE3, SSE4.1/4.2, SSE4a, FMA3, FMA4, XOP, AVX, AVX2 or AVX-512.

The "unmanaged" library is provided as separate 32&#8209;Bit (x86) and 64&#8209;Bit (x64) DLL files, allowing it to work on *both* platforms; *AnyCPU* assemblies automatically use the appropriate DLL.


Background
==========

To the best of my knowledge, the Microsoft .NET Framework does **not** provide a *built-in* way to acquire detailed information about the underlying CPU. The [`Environment`](https://docs.microsoft.com/de-de/dotnet/api/system.environment?view=netframework-4.5) class can be used to check wether we are running on a 64&#8209;Bit operating system, which would imply that we are running on a 64&#8209;Bit (x64) processor, but it does **not** provide any further information about the CPU. The Microsoft .NET Framework also provides APIs for consuming data and events from the WMI (Windows Management Instrumentation) infrastructure. Unfortunately, even though the the [`Win32_Processor`](https://docs.microsoft.com/en-us/windows/win32/cimwin32prov/win32-processor) WMI class can provide various information about the CPU, it does **not** reveal the supported CPU capabilities (i.e. instruction set extensions).

If using *P/Invoke* is an option, then we can query the *Win32 API* directly. The functions [`GetNativeSystemInfo()`](https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getnativesysteminfo) and especially [`IsProcessorFeaturePresent()`](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-isprocessorfeaturepresent) look promising at a first glance, but it quickly becomes evident that these functions do **not** support "modern" CPU instruction set extensions, such as AVX, AVX2 or AVX-512. There also is the [`GetEnabledXStateFeatures()`](https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getenabledxstatefeatures) function, which may be used to detect whether AVX support is enabled on the operating system-level, which would imply that at least AVX is supported by the CPU. But, unfortunately, there is **no** way to detect whether AVX2 or AVX-512 are supported too. The Win32 API does **not** currently provide a way to detect AVX2 or AVX-512.

After all, using the [`CPUID`](https://en.wikipedia.org/wiki/CPUID) instruction is the only reliable way to detect the supported CPU capabilities, including all the instruction set extensions that were are interested in. The [`XGETBV`](https://www.felixcloutier.com/x86/xgetbv) instruction is also required, in order to test whether AVX support is enabled on the operating system-level. However, since it is **not** possible to call these instructions directly from "managed" code in the Microsoft .NET Framework, it becomes necessary to use an "unmanaged" helper library for this purpose. This is exactly how the *CPUCapabilities.NET* library works!


Managed API
===========

Information about the CPU are provided by the **`CPU`** class, which is located in the `Muldersoft.CPUCapabilitiesDotNet` namespace.

The following *read-only* properties are available:

* **`Architecture`** &ndash; The architecture of the CPU. This currently can be one of the following values:
  * `CPU_ARCH_X86`: [x86](https://en.wikipedia.org/wiki/IA-32) architecture (32&#8209;Bit), also known as "i386" or "IA-32"
  * `CPU_ARCH_X64`: [x64](https://en.wikipedia.org/wiki/X86-64) architecture (64&#8209;Bit), also known as "x86-64", "AMD64" or "Intel 64" (formerly "EM64T")

  *Note:* This library reports the "usable" CPU architecture. On a 64&#8209;Bit operating system, the CPU architecture will be reported as x64 (64&#8209;Bit), regardless of whether the current process is 32&#8209;Bit or 64&#8209;Bit. On a 32&#8209;Bit operating system, the CPU architecture will be reported as x86 (32&#8209;Bit), even if the underlying CPU does support the x64 (64&#8209;Bit) extension in hardware.

* **`Capabilities`** &ndash; The capabilities supported by the CPU. This can be the OR-combination of any of the following flags:
  * `CPU_3DNOW`:
  * `CPU_3DNOWEXT`:
  * `CPU_AES`:
  * `CPU_AVX`:
  * `CPU_AVX2`:
  * `CPU_AVX512_BW`:
  * `CPU_AVX512_CD`:
  * `CPU_AVX512_DQ`:
  * `CPU_AVX512_ER`:
  * `CPU_AVX512_F`:
  * `CPU_AVX512_IFMA`:
  * `CPU_AVX512_PF`:
  * `CPU_AVX512_VL`:
  * `CPU_BMI1`:
  * `CPU_BMI2`:
  * `CPU_FMA3`:
  * `CPU_FMA4`:
  * `CPU_LZCNT`:
  * `CPU_MMX`:
  * `CPU_MMXEXT`:
  * `CPU_POPCNT`:
  * `CPU_RDRND`:
  * `CPU_RDSEED`:
  * `CPU_SHA`:
  * `CPU_SSE`:
  * `CPU_SSE2`:
  * `CPU_SSE3`:
  * `CPU_SSE41`:
  * `CPU_SSE42`:
  * `CPU_SSE4a`:
  * `CPU_SSSE3`:
  * `CPU_XOP`:

  *Note:* This library reports the "usable" CPU capabilities. Capabilities are reported as supported, if and only if they are supported by both, the CPU and the operating system. For example, the AVX instruction set extension requires support by the operating system, but AVX support was **not** added until Windows 7 with SP-1 or Windows Server 2008 R2 with SP1.


License
=======

This work has been released under the **CC0 1.0 Universal** license.

For details, please refer to:  
<https://creativecommons.org/publicdomain/zero/1.0/legalcode>
