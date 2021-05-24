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

All information are provided by a set of *read-only* properties. The following properties are available:

* **`Architecture`** &ndash; The architecture of the CPU:
  * `CPU_ARCH_X86`: [x86](https://en.wikipedia.org/wiki/IA-32) architecture (32&#8209;Bit), also known as "i386" or "IA-32"
  * `CPU_ARCH_X64`: [x64](https://en.wikipedia.org/wiki/X86-64) architecture (64&#8209;Bit), also known as "x86-64", "AMD64" or "Intel 64" (formerly "EM64T")
  * ***Note:*** This library reports the "usable" CPU architecture. On a 64&#8209;Bit operating system, the CPU architecture will be reported as x64 (64&#8209;Bit), regardless of whether the current process is 32&#8209;Bit or 64&#8209;Bit. On a 32&#8209;Bit operating system, the CPU architecture will be reported as x86 (32&#8209;Bit), even if the underlying CPU *does* support the x64 (64&#8209;Bit) extensions.

* **`Count`** &ndash; The number of available processors (CPU cores).  
  *  ***Note:*** For CPUs with HyperThreading (Intel) or Simultaneous Multithreading (AMD), this value is equal to the number of "logical" CPU cores.

* **`Vendor`** &ndash; The CPU vendor ID string, 12 characters in length, for example:
  * `GenuineIntel`: [Intel](https://en.wikipedia.org/wiki/Intel)
  * `AuthenticAMD`: [AMD](https://en.wikipedia.org/wiki/Advanced_Micro_Devices)
  * `CyrixInstead`: [Cyrix](https://en.wikipedia.org/wiki/Cyrix)

* **`Information`** &ndash; The [CPU identifier](https://en.wikipedia.org/wiki/CPUID#EAX=1:_Processor_Info_and_Feature_Bits), which is composed of the following fields:
  * `Type`: The CPU type (e.g. OEM Processor, Intel Overdrive Processor or Dual processor)
  * `Family`: The effective CPU family ID, as computed from the CPU's "family ID" and "extended family ID" values
  * `Model`: The effective CPU model ID, as computed from the CPU's "family ID", "model ID" and "extended model ID" values
  * `Stepping`: The CPU stepping, i.e. product revision number (errata or other changes)
  * `RawFamily`: The (basic) CPU family ID &ndash; raw value
  * `RawFamilyExt`: The extended CPU family ID  &ndash; raw value
  * `RawModel`: The (basic) CPU model ID &ndash; raw value
  * `RawModelExt`: The extended CPU model ID &ndash; raw value

* **`Capabilities`** &ndash; The capabilities  (instruction set extensions) supported by the CPU. This can be the bitwise OR combination of any of the following capability flags:
  * `CPU_3DNOW`: [3DNow!](https://en.wikipedia.org/wiki/3DNow!)
  * `CPU_3DNOWEXT`: [Extended 3DNow!](https://en.wikipedia.org/wiki/3DNow!#3DNow_extensions)
  * `CPU_ADX`: [Multi-Precision Add-Carry Instruction Extensions (ADX)](https://en.wikipedia.org/wiki/Intel_ADX)
  * `CPU_AES`: [Advanced Encryption Standard (AES) instruction set](https://en.wikipedia.org/wiki/AES_instruction_set)
  * `CPU_AVX`: [Advanced Vector Extensions (AVX)](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions)
  * `CPU_AVX2`: [Advanced Vector Extensions 2 (AVX2)](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions#Advanced_Vector_Extensions_2)
  * `CPU_AVX512_BITALG`: [AVX-512](https://en.wikipedia.org/wiki/AVX-512#Instruction_set) &ndash; (BITALG)
  * `CPU_AVX512_BW`: [AVX-512](https://en.wikipedia.org/wiki/AVX-512#Instruction_set) &ndash; Byte and Word Instructions (BW)
  * `CPU_AVX512_CD`: [AVX-512](https://en.wikipedia.org/wiki/AVX-512#Instruction_set) &ndash; Conflict Detection Instructions (CD)
  * `CPU_AVX512_DQ`: [AVX-512](https://en.wikipedia.org/wiki/AVX-512#Instruction_set) &ndash; Doubleword and Quadword Instructions (DQ)
  * `CPU_AVX512_ER`: [AVX-512](https://en.wikipedia.org/wiki/AVX-512#Instruction_set) &ndash; Exponential and Reciprocal Instructions (ER)
  * `CPU_AVX512_F`: [AVX-512](https://en.wikipedia.org/wiki/AVX-512#Instruction_set) &ndash; Foundation (F)
  * `CPU_AVX512_IFMA`: [AVX-512](https://en.wikipedia.org/wiki/AVX-512#Instruction_set) &ndash; Integer Fused Multiply Add (IFMA)
  * `CPU_AVX512_PF`: [AVX-512](https://en.wikipedia.org/wiki/AVX-512#Instruction_set) &ndash; Prefetch Instructions (PF)
  * `CPU_AVX512_VBMI`: [AVX-512](https://en.wikipedia.org/wiki/AVX-512#Instruction_set) &ndash; Vector Bit Manipulation Instructions (VBMI)
  * `CPU_AVX512_VBMI2`: [AVX-512](https://en.wikipedia.org/wiki/AVX-512#Instruction_set) &ndash; Vector Bit Manipulation Instructions 2 (VBMI2)
  * `CPU_AVX512_VL`: [AVX-512](https://en.wikipedia.org/wiki/AVX-512#Instruction_set) &ndash; Vector Length Extensions (VL)
  * `CPU_AVX512_VNNI`: [AVX-512](https://en.wikipedia.org/wiki/AVX-512#Instruction_set) &ndash; Vector Neural Network Instructions (VNNI)
  * `CPU_AVX512_VPOPCNTDQ`: [AVX-512](https://en.wikipedia.org/wiki/AVX-512#Instruction_set) &ndash; Vector Population Count Double and Quad-word (VPOPCNTDQ)
  * `CPU_BMI1`: [Bit Manipulation Instruction Set 1 (BMI1)](https://en.wikipedia.org/wiki/Bit_manipulation_instruction_set#BMI1_(Bit_Manipulation_Instruction_Set_1))
  * `CPU_BMI2`: [Bit Manipulation Instruction Set 2 (BMI2)](https://en.wikipedia.org/wiki/Bit_manipulation_instruction_set#BMI2_(Bit_Manipulation_Instruction_Set_2))
  * `CPU_CLFSH`: [CLFLUSH](https://www.felixcloutier.com/x86/clflush) instruction (SSE2)
  * `CPU_CMOV`: [CMOV](https://www.felixcloutier.com/x86/cmovcc) and [FCMOV](https://en.wikipedia.org/wiki/FCMOV) instructions
  * `CPU_CX16`: [CMPXCHG16B](https://www.felixcloutier.com/x86/cmpxchg8b:cmpxchg16b) instruction
  * `CPU_CX8`: [CMPXCHG8B](https://www.felixcloutier.com/x86/cmpxchg8b:cmpxchg16b) instruction
  * `CPU_ERMS`: Enhanced [REP](https://www.felixcloutier.com/x86/rep:repe:repz:repne:repnz) [MOVSB](https://www.felixcloutier.com/x86/movs:movsb:movsw:movsd:movsq) and [STOSB](https://www.felixcloutier.com/x86/stos:stosb:stosw:stosd:stosq) instructions
  * `CPU_FMA3`: [Fused multiply–Add &ndash; *three* operands version (FMA3)](https://en.wikipedia.org/wiki/FMA_instruction_set#FMA3_instruction_set)
  * `CPU_FMA4`: [Fused Multiply–Add &ndash; *four* operands version (FMA4)](https://en.wikipedia.org/wiki/FMA_instruction_set#FMA4_instruction_set)
  * `CPU_FPU`: [x87 FPU](https://en.wikipedia.org/wiki/X87)
  * `CPU_FSGSBASE`: [FSGSBASE](https://lwn.net/Articles/821723/) &ndash; directly manipulate the FS and GS base registers
  * `CPU_FXSR`: [FXSAVE](https://www.felixcloutier.com/x86/fxsave) and [FXRSTOR](https://www.felixcloutier.com/x86/fxrstor) instructions
  * `CPU_HLE`: [Transactional Synchronization Extensions (TSX)](https://en.wikipedia.org/wiki/Transactional_Synchronization_Extensions) &ndash; Hardware Lock Elision (HLE)
  * `CPU_HTT`: [Hyper-Threading Technology](https://en.wikipedia.org/wiki/Hyper-threading)
  * `CPU_INVPCID`: [INVPCID](https://www.felixcloutier.com/x86/invpcid) instruction
  * `CPU_LAHF`: [LAHF](https://www.felixcloutier.com/x86/lahf) and [SAHF](https://www.felixcloutier.com/x86/sahf) instructions in long mode
  * `CPU_LZCNT`: [Leading Zeros Count (LZCNT)](https://en.wikipedia.org/wiki/Bit_manipulation_instruction_set#ABM_(Advanced_Bit_Manipulation))
  * `CPU_MMX`: [Multi Media Extension (MMX)](https://en.wikipedia.org/wiki/MMX_(instruction_set))
  * `CPU_MMXEXT` [Extended MMX](https://en.wikipedia.org/wiki/Extended_MMX) &ndash; subset of SSE instructions (extended MMX instructions) supported by [Athlon](https://en.wikipedia.org/wiki/Athlon) microarchitecture (and later)
  * `CPU_MONITOR`: [MONITOR](https://www.felixcloutier.com/x86/monitor) and [MWAIT](https://www.felixcloutier.com/x86/mwait) instructions (SSE3)
  * `CPU_MOVBE`: [MOVBE](https://www.felixcloutier.com/x86/movbe) instruction
  * `CPU_MSR`: [Model-Specific Register (MSR)](https://en.wikipedia.org/wiki/Model-specific_register)
  * `CPU_OSXSAVE`: [XSAVE](https://www.felixcloutier.com/x86/xsave) enabled by OS 
  * `CPU_PCLMULQDQ`: [PCLMULQDQ](https://en.wikipedia.org/wiki/CLMUL_instruction_set) instruction
  * `CPU_POPCNT`: [Population Count (POPCNT)](https://en.wikipedia.org/wiki/Bit_manipulation_instruction_set#ABM_(Advanced_Bit_Manipulation))
  * `CPU_PREFETCHWT1`: [PREFETCHWT1](https://www.felixcloutier.com/x86/prefetchwt1) instruction
  * `CPU_RDRND`: [Hardware-generated random value (RDRND)](https://en.wikipedia.org/wiki/RDRAND) &ndash; Intel Secure Key Technology
  * `CPU_RDSEED`: [Hardware-generated random seed value (RDSEED)](https://software.intel.com/content/www/us/en/develop/blogs/the-difference-between-rdrand-and-rdseed.html) &ndash; Intel Secure Key Technology
  * `CPU_RDTSCP`: [RDTSCP](https://www.felixcloutier.com/x86/rdtscp) instruction
  * `CPU_RTM`: [Transactional Synchronization Extensions (TSX)](https://en.wikipedia.org/wiki/Transactional_Synchronization_Extensions) &ndash; Restricted Transactional Memory (RTM)
  * `CPU_SEP`: [SYSENTER](https://www.felixcloutier.com/x86/sysenter) and [SYSEXIT](https://www.felixcloutier.com/x86/sysexit) instructions
  * `CPU_SHA`: [Intel SHA extensions](https://en.wikipedia.org/wiki/Intel_SHA_extensions)
  * `CPU_SSE`: [Streaming SIMD Extensions (SSE)](https://en.wikipedia.org/wiki/Streaming_SIMD_Extensions)
  * `CPU_SSE2`: [Streaming SIMD Extensions 2 (SSE2)](https://en.wikipedia.org/wiki/SSE2)
  * `CPU_SSE3`: [Streaming SIMD Extensions 3 (SSE3)](https://en.wikipedia.org/wiki/SSE3)
  * `CPU_SSE41`: [Streaming SIMD Extensions 4.1 (SSE4.1)](https://en.wikipedia.org/wiki/SSE4#SSE4.1) &ndash; subset of SSE4 supported by [Penryn](https://en.wikipedia.org/wiki/Intel_Core_2#Penryn) microarchitecture (and later)
  * `CPU_SSE42`: [Streaming SIMD Extensions 4.2 (SSE4.2)](https://en.wikipedia.org/wiki/SSE4#SSE4.2) &ndash; subset of SSE4 supported by [Nehalem](https://en.wikipedia.org/wiki/Nehalem_(microarchitecture)) microarchitecture (and later)
  * `CPU_SSE4a`: [Streaming SIMD Extensions 4a](https://en.wikipedia.org/wiki/SSE4#SSE4a) &ndash; subset of SSE4 supported by AMD only, **not** supported by Intel
  * `CPU_SSSE3`: [Supplemental Streaming SIMD Extensions 3 (SSSE3)](https://en.wikipedia.org/wiki/SSSE3)
  * `CPU_SYSCALL`: [SYSCALL](https://www.felixcloutier.com/x86/syscall) instruction
  * `CPU_TBM`: [Trailing Bit Manipulation (TBM)](https://en.wikipedia.org/wiki/Bit_manipulation_instruction_set#TBM)
  * `CPU_TSC`: [Time Stamp Counter (TSC)](https://en.wikipedia.org/wiki/Time_Stamp_Counter)
  * `CPU_XOP`: [eXtended Operations (XOP)](https://en.wikipedia.org/wiki/XOP_instruction_set)
  * `CPU_XSAVE`: [XSAVE](https://www.felixcloutier.com/x86/xsave) instruction
  * ***Note:*** This library reports the "usable" CPU capabilities. Capabilities are reported as supported, if and only if they are supported by both, the CPU and the operating system. For example, the AVX instruction set extension requires support by the operating system, but support for AVX was **not** added to Windows until Windows 7 with SP-1.

* **`Brand`** &ndash; The CPU brand string, which is *at most* 48 characters in length, for example:
  * `AMD Ryzen 9 3900X 12-Core Processor`
  * `Intel(R) Core(TM) i7-6700K CPU @ 4.00GHz`


License
=======

This work has been released under the **CC0 1.0 Universal** license.

For details, please refer to:  
<https://creativecommons.org/publicdomain/zero/1.0/legalcode>
