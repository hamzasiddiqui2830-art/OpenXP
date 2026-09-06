# ReactOS-WRK - Multi-Compiler Build System

This document describes the enhanced build system for the ReactOS-WRK that supports multiple compilers and architectures.

## Features

### Compiler Support
- **Visual Studio 2022** (MSVC) - Native Windows development
- **MinGW-w64** - Cross-compilation from Linux/Windows
- **GCC** - Native or cross-compilation
- **Clang** - Alternative compiler with MSVC compatibility
- **LLVM/Clang with LLD** - Full LLVM toolchain

### Architecture Support
- **x86 (i386)** - 32-bit architecture
- **amd64 (x64)** - 64-bit architecture

### Output Directory Structure
Build outputs are organized by compiler and architecture:
```
output-VS-i386/      # Visual Studio 32-bit build
output-VS-amd64/     # Visual Studio 64-bit build
output-MinGW-i386/   # MinGW 32-bit build
output-MinGW-amd64/  # MinGW 64-bit build
output-GCC-i386/     # GCC 32-bit build
output-GCC-amd64/    # GCC 64-bit build
output-LLVM-i386/    # LLVM 32-bit build
output-LLVM-amd64/   # LLVM 64-bit build
```

## HAL Stub Implementation

A stub Hardware Abstraction Layer (`hal_stub.c`) has been created in the root directory to eliminate dependency on precompiled HAL DLLs. This provides:

- `HalPrivateDispatchTable` - Main HAL dispatch table
- Stub implementations of common HAL functions
- All functions return safe defaults or STATUS_NOT_IMPLEMENTED

## Assembly File Handling

The build system automatically handles assembly files:
- **MSVC**: Uses MASM-style `.asm` files directly
- **MinGW/GCC/Clang/LLVM**: Converts `.asm` files to `.S` format for GAS assembler

Note: The conversion is a simple copy. For production use, you may need to convert MASM syntax to GAS syntax manually.

## Building

### Using the Build Script (Recommended)

```bash
cd /workspace/build_scripts

# Build with MinGW for x86
./build-wrk.sh mingw i386

# Build with MinGW for amd64
./build-wrk.sh mingw amd64

# Build with GCC for x86
./build-wrk.sh gcc i386

# Build with Clang for amd64
./build-wrk.sh clang amd64

# Build with LLVM for x86
./build-wrk.sh llvm i386

# For Visual Studio (run on Windows)
./build-wrk.sh vs i386
./build-wrk.sh vs amd64
```

### Using CMake Directly

#### MinGW (Windows or Linux)
```bash
# For x86
mkdir build-mingw-x86 && cd build-mingw-x86
cmake -G "MinGW Makefiles" \
      -DWRK_ARCH=x86 \
      -DCMAKE_C_COMPILER=i686-w64-mingw32-gcc \
      ..
make -j$(nproc)

# For amd64
mkdir build-mingw-x64 && cd build-mingw-x64
cmake -G "MinGW Makefiles" \
      -DWRK_ARCH=amd64 \
      -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
      ..
make -j$(nproc)
```

#### GCC (Linux)
```bash
# For x86
mkdir build-gcc-x86 && cd build-gcc-x86
cmake -G "Unix Makefiles" \
      -DWRK_ARCH=x86 \
      -DCMAKE_C_FLAGS="-m32" \
      ..
make -j$(nproc)
```

#### Visual Studio (Windows)
```powershell
# For x86
cmake -G "Visual Studio 17 2022" -A Win32 -DWRK_ARCH=x86 ..
cmake --build . --config Release

# For amd64
cmake -G "Visual Studio 17 2022" -A x64 -DWRK_ARCH=amd64 ..
cmake --build . --config Release
```

#### LLVM/Clang with LLD
```bash
# For x86
mkdir build-llvm-x86 && cd build-llvm-x86
cmake -G "Ninja" \
      -DWRK_ARCH=x86 \
      -DCMAKE_C_COMPILER=clang \
      -DCMAKE_LINKER=lld-link \
      -DCMAKE_C_FLAGS="-m32" \
      ..
ninja
```

## Files Modified/Created

### Created Files
1. `/workspace/hal_stub.c` - HAL stub implementation
2. `/workspace/build_scripts/build-wrk.sh` - Unified build script
3. `/workspace/BUILDING.md` - This documentation

### Modified Files
1. `/workspace/CMakeLists.txt`
   - Added ASM language support
   - Compiler detection (MSVC, MinGW, GCC, Clang, LLVM)
   - Output directory naming with compiler prefix
   - HAL stub library integration
   - Compiler-specific entry point handling

2. `/workspace/cmake/modules/WRKCompilerFlags.cmake`
   - Enhanced compiler flag support for all compilers
   - Architecture-specific flags
   - Linker flags for each compiler type

3. `/workspace/cmake/modules/WRKModuleBuilder.cmake`
   - Assembly file collection and conversion
   - Compiler-architecture output directory naming
   - Support for .asm to .S conversion

## Requirements

### For MinGW Cross-Compilation
```bash
# Ubuntu/Debian
sudo apt-get install mingw-w64

# Fedora/RHEL
sudo dnf install mingw-w64
```

### For GCC
```bash
# Ubuntu/Debian (32-bit support)
sudo apt-get install gcc-multilib g++-multilib

# Fedora/RHEL
sudo dnf install gcc glibc-devel.i686
```

### For Clang/LLVM
```bash
# Ubuntu/Debian
sudo apt-get install clang lld

# Fedora/RHEL
sudo dnf install clang lld
```

### For Visual Studio
- Visual Studio 2022 with C++ desktop development workload
- Windows SDK

## Troubleshooting

### Assembly Syntax Errors
The current build copies `.asm` files as `.S` files without syntax conversion. MASM and GAS have different syntax. You may need to:
1. Manually convert assembly files from MASM to GAS syntax
2. Or use only MSVC for building until conversion is complete

### Missing Headers
Ensure all required SDK headers are present in:
- `/workspace/sdk/ddk/inc/`
- `/workspace/sdk/internal/ds/inc/`
- `/workspace/sdk/sdk/inc/`
- `/workspace/ntoskrnl/inc/`

### Linker Errors
If you encounter undefined references to HAL functions, ensure `hal_stub.c` is being compiled and linked.

## Notes

1. This is a research kernel and may not boot on real hardware
2. Assembly file conversion requires manual intervention for full compatibility
3. Some HAL stubs return minimal implementations - functionality may be limited
4. The build system is designed for research and educational purposes

## License

Copyright (c) OpenXP Team. All rights reserved.
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
