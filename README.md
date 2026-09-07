# ReactOS-WRK

ReactOS Windows Research Kernel (WRK) - A free and open-source operating system compatible with Windows applications and drivers.

## Overview

This project is based on the Windows Research Kernel (WRK) and aims to create a fully functional, open-source operating system that can run Windows applications and drivers natively. The codebase includes implementations of core Windows kernel components including:

- **NTOSKRNL**: NT OS Kernel with process/thread management, memory management, I/O subsystem
- **HAL**: Hardware Abstraction Layer for different architectures
- **LPC**: Local Procedure Call mechanism for inter-process communication
- **Cache Manager**: File system caching subsystem
- **FSRTL**: File System Run-Time Library
- **PNP Manager**: Plug and Play device management

## License

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

## Building

### Prerequisites

- GCC 15 or later (for Linux builds)
- CMake 3.20 or later
- Python 3.x
- NASM (for assembly files)

### Architecture Support

- x86 (i386) - Primary development target
- AMD64 (x86-64) - In progress
- ARM (32-bit) - Supported
- ARM64 (AArch64) - Supported
- IA64 (Itanium) - Removed (legacy architecture)

### Build Instructions

#### Building with GCC (Linux)

```bash
cd /workspace
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DARCH=x86
make
```

#### Building one component at a time

To reduce error margin and isolate issues, you can build individual components:

```bash
cd build
make ntos_lpc        # Build LPC subsystem
make ntos_ke         # Build Kernel Executive
make ntos_io         # Build I/O Manager
make ntos_mm         # Build Memory Manager
make ntos_cache      # Build Cache Manager
```

### Compiler Compatibility

#### GCC Compatibility Macros

The project includes compatibility macros to support building with GCC while maintaining MSVC source compatibility:

- `__forceinline` → `inline __attribute__((always_inline))`
- `__declspec(x)` → `__attribute__((x))`
- Calling conventions (`__stdcall`, `__fastcall`, `__cdecl`)
- SEH macros (Structured Exception Handling) - implemented as no-ops for GCC
- Architecture detection for ARM/ARM64 when using GCC

**Note**: SEH (`try`/`except`/`finally`) is implemented as no-op macros when compiling with GCC, as GCC does not support MSVC-style Structured Exception Handling. This may affect error handling behavior in some code paths.

**ARM/ARM64 Support**: The project now includes full GCC compatibility for ARM and ARM64 architectures, including:
- Memory barriers using ARM assembly instructions
- Cache flush operations
- Atomic operations using GCC built-ins
- Interrupt level management

#### MSVC Compatibility

When building with MSVC, the standard Windows SDK headers and compiler intrinsics are used. Ensure you have:

- Visual Studio 2019 or later
- Windows 10 SDK or later

## Directory Structure

```
/workspace
├── ntoskrnl/          # NT OS Kernel
│   ├── inc/          # Internal kernel headers
│   ├── cache/        # Cache manager
│   ├── ke/           # Kernel executive
│   ├── io/           # I/O manager
│   ├── lpc/          # Local procedure call
│   ├── mm/           # Memory manager
│   ├── ob/           # Object manager
│   ├── ps/           # Process support
│   └── se/           # Security reference monitor
├── hals/             # Hardware Abstraction Layers
├── sdk/              # Software Development Kit
│   └── sdk/inc/      # Public SDK headers
├── cmake/            # CMake build configuration
└── output-VS-i386/   # Build output (VS builds)
```

## Key Components

### NTOSKRNL (NT OS Kernel)

The main kernel executable providing:
- Process and thread management
- Virtual memory management
- I/O request processing
- Object management
- Security enforcement
- Inter-process communication (LPC)

### HAL (Hardware Abstraction Layer)

Provides hardware-specific implementations for:
- Interrupt handling
- DMA operations
- Bus enumeration
- Timer services
- CPU-specific operations

### LPC (Local Procedure Call)

Lightweight IPC mechanism for:
- Client-server communication
- Message passing between processes
- Port-based communication channels

## Known Issues

1. **SEH Support**: When building with GCC, Structured Exception Handling is implemented as no-op macros. Code paths relying on exception handling may not behave correctly.

2. **Pragma Warnings**: MSVC-specific `#pragma warning` directives are ignored when compiling with GCC.

3. **Architecture Definitions**: Ensure proper architecture flags are set during CMake configuration (e.g., `-D_M_IX86` for x86).

## Contributing

Contributions are welcome! Please follow these guidelines:

1. Maintain GPL v3+ license compatibility
2. Follow existing code style and conventions
3. Test changes on supported architectures
4. Document new features and known limitations

## Development Status

This is an active research and development project. Many components are functional but may not be production-ready. Key areas of ongoing development include:

- Complete SEH support for GCC toolchain
- Full PnP manager implementation
- Advanced power management features
- Multi-processor synchronization improvements

## References

- [Windows Research Kernel Documentation](https://www.microsoft.com/en-us/research/project/windows-research-kernel/)
- [ReactOS Project](https://reactos.org/)
- [GNU Compiler Collection](https://gcc.gnu.org/)

## Contact

For questions and discussions, please open an issue on the project repository.

---

**Disclaimer**: This project is for educational and research purposes. It is not affiliated with Microsoft Corporation. Windows and NT are trademarks of Microsoft Corporation.
