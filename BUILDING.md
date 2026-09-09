# OpenXP Multi-Compiler Build System

This document describes the OpenXP build system and its supported compilers and architectures.

## Features

### Compiler Support
- **Visual Studio** (MSVC) - Native Windows development
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
output-vs-i386/
output-vs-amd64/
output-mingw-i386/
output-mingw-amd64/
output-gcc-i386/
output-gcc-amd64/
output-llvm-i386/
output-llvm-amd64/
```

## HAL Stub Implementation

A stub Hardware Abstraction Layer (`hal_stub.c`) is provided to eliminate dependency on precompiled HAL DLLs. It provides safe defaults or `STATUS_NOT_IMPLEMENTED` for common HAL entry points.

## Assembly File Handling

The build system automatically handles assembly files:
- **MSVC**: Uses MASM-style `.asm` files directly
- **MinGW/GCC/Clang/LLVM**: Copies `.asm` files to `.S` files for the GAS toolchain

Note: the conversion is currently a file-format copy. MASM and GAS syntax are not identical, so non-MSVC builds may require additional assembly conversion work.

## Building

### Using the configure script (recommended)

The build entry point now lives in the project root.

```bash
# Auto-detect compiler and architecture
./configure.sh

# Explicit compiler and architecture
./configure.sh mingw i386
./configure.sh mingw amd64
./configure.sh gcc i386
./configure.sh clang amd64
./configure.sh llvm i386
```

On Windows, use the command script from a normal Command Prompt or a Visual Studio Developer Command Prompt:

```cmd
configure.cmd
configure.cmd vs i386
configure.cmd vs amd64
configure.cmd clang amd64
configure.cmd mingw i386
```

The Windows script is designed to work with the MSVC environment supplied by the Visual Studio Developer Command Prompt. In CI, the workflow initializes the matching MSVC environment before invoking `configure.cmd`.

### Using CMake directly

The configure scripts are the preferred entry point, but CMake can still be invoked directly when needed.

#### MinGW
```bash
cmake -G Ninja \
      -DWRK_ARCH=x86 \
      -DCMAKE_C_COMPILER=i686-w64-mingw32-gcc \
      -DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++ \
      -B output-mingw-i386/build \
      -S .
cmake --build output-mingw-i386/build --parallel
```

#### GCC
```bash
cmake -G Ninja \
      -DWRK_ARCH=x86 \
      -DCMAKE_C_COMPILER=gcc \
      -DCMAKE_CXX_COMPILER=g++ \
      -DCMAKE_C_FLAGS=-m32 \
      -B output-gcc-i386/build \
      -S .
cmake --build output-gcc-i386/build --parallel
```

#### Visual Studio / MSVC
```cmd
configure.cmd vs i386
configure.cmd vs amd64
```

The CI build uses the same root configure entry point for both MSVC architectures.

#### LLVM/Clang with LLD
```bash
cmake -G Ninja \
      -DWRK_ARCH=x86 \
      -DCMAKE_C_COMPILER=clang \
      -DCMAKE_LINKER=lld-link \
      -DCMAKE_C_FLAGS=-m32 \
      -B output-llvm-i386/build \
      -S .
cmake --build output-llvm-i386/build --parallel
```

## Build Entry Points

- `/configure.sh` - Unix-like environments
- `/configure.cmd` - Windows environments
- `/.github/workflows/visual-studio-build.yml` - CI validation for MSVC x86 and amd64

The old `build_scripts/build-wrk.sh` entry point has been removed; use the root configure scripts instead.

## Build-System Files

- `/CMakeLists.txt` - OpenXP top-level CMake configuration
- `/cmake/modules/WRKCompilerFlags.cmake` - compiler and architecture flags
- `/cmake/modules/WRKModuleBuilder.cmake` - kernel module source and assembly handling

The two `WRK*.cmake` module names are retained for now because they are implementation filenames used by the existing CMake configuration; their contents are part of the OpenXP build system.

## Requirements

### MinGW Cross-Compilation
```bash
# Ubuntu/Debian
sudo apt-get install mingw-w64
```

### GCC
```bash
# Ubuntu/Debian (32-bit support)
sudo apt-get install gcc-multilib g++-multilib
```

### Clang/LLVM
```bash
# Ubuntu/Debian
sudo apt-get install clang lld
```

### Visual Studio
- Visual Studio with C++ desktop development workload
- Windows SDK
- Ninja
- CMake

## Troubleshooting

### Visual Studio compiler not detected
Run `configure.cmd` from a Visual Studio Developer Command Prompt, or pass `vs` explicitly after initializing the MSVC environment.

### Assembly syntax errors
The current non-MSVC path copies `.asm` files to `.S` without syntax conversion. Use MSVC for the most complete assembly compatibility until the GAS conversion is finished.

### Missing headers
Ensure the required SDK and kernel headers are present under `sdk/` and `ntoskrnl/inc/`.

### Linker errors
If HAL functions are unresolved, ensure the `hal_stub` target is enabled and included by the top-level CMake configuration.

## Notes

1. OpenXP is a research kernel and may not boot on real hardware.
2. Assembly conversion requires additional work for full non-MSVC compatibility.
3. Some HAL stubs intentionally provide minimal implementations.
4. The build system is intended for research and educational use.

## License

Copyright (c) OpenXP Team. All rights reserved.
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
