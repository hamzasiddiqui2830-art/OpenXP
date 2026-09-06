#!/bin/bash
# Build script for Windows Research Kernel (WRK)
# Supports multiple compilers and architectures
# Usage: ./build-wrk.sh [compiler] [architecture]
#   compiler: mingw, gcc, clang, vs, llvm (default: mingw)
#   architecture: i386, amd64 (default: i386)

set -e

# Default values
COMPILER="${1:-mingw}"
ARCH="${2:-i386}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_DIR="$(dirname "$SCRIPT_DIR")"

# Validate compiler
case "$COMPILER" in
    mingw|gcc|clang|vs|llvm)
        ;;
    *)
        echo "Error: Unsupported compiler '$COMPILER'"
        echo "Supported compilers: mingw, gcc, clang, vs, llvm"
        exit 1
        ;;
esac

# Validate architecture
case "$ARCH" in
    i386|amd64)
        ;;
    *)
        echo "Error: Unsupported architecture '$ARCH'"
        echo "Supported architectures: i386, amd64"
        exit 1
        ;;
esac

# Set output directory based on compiler and architecture
OUTPUT_DIR="$WORKSPACE_DIR/output-${COMPILER}-${ARCH}"

echo "=============================================="
echo "Windows Research Kernel Build Script"
echo "=============================================="
echo "Compiler: $COMPILER"
echo "Architecture: $ARCH"
echo "Output Directory: $OUTPUT_DIR"
echo "=============================================="

# Clean output directory if it exists
if [ -d "$OUTPUT_DIR" ]; then
    echo "Cleaning existing output directory..."
    rm -rf "$OUTPUT_DIR"
fi

mkdir -p "$OUTPUT_DIR"

# Set up compiler-specific variables
case "$COMPILER" in
    mingw)
        if [ "$ARCH" = "i386" ]; then
            CC="i686-w64-mingw32-gcc"
            CXX="i686-w64-mingw32-g++"
            AS="i686-w64-mingw32-as"
            LD="i686-w64-mingw32-ld"
            ARCH_FLAGS="-m32"
        else
            CC="x86_64-w64-mingw32-gcc"
            CXX="x86_64-w64-mingw32-g++"
            AS="x86_64-w64-mingw32-as"
            LD="x86_64-w64-mingw32-ld"
            ARCH_FLAGS="-m64"
        fi
        CMAKE_GENERATOR="MinGW Makefiles"
        ;;
    gcc)
        CC="gcc"
        CXX="g++"
        AS="as"
        LD="ld"
        if [ "$ARCH" = "i386" ]; then
            ARCH_FLAGS="-m32"
        else
            ARCH_FLAGS="-m64"
        fi
        CMAKE_GENERATOR="Unix Makefiles"
        ;;
    clang)
        CC="clang"
        CXX="clang++"
        AS="llvm-as"
        LD="lld"
        if [ "$ARCH" = "i386" ]; then
            ARCH_FLAGS="-m32"
        else
            ARCH_FLAGS="-m64"
        fi
        CMAKE_GENERATOR="Unix Makefiles"
        ;;
    llvm)
        CC="clang"
        CXX="clang++"
        AS="llvm-as"
        LD="lld-link"
        if [ "$ARCH" = "i386" ]; then
            ARCH_FLAGS="-m32"
        else
            ARCH_FLAGS="-m64"
        fi
        CMAKE_GENERATOR="Ninja"
        ;;
    vs)
        # Visual Studio - will use cmake to generate VS solution
        CC="cl"
        CXX="cl"
        AS="ml"
        LD="link"
        ARCH_FLAGS=""
        if [ "$ARCH" = "i386" ]; then
            CMAKE_GENERATOR="Visual Studio 17 2022"
            CMAKE_ARCH="Win32"
        else
            CMAKE_GENERATOR="Visual Studio 17 2022"
            CMAKE_ARCH="x64"
        fi
        ;;
esac

# Export compiler variables
export CC="$CC"
export CXX="$CXX"

echo ""
echo "Configuring build with CMake..."
echo "Generator: $CMAKE_GENERATOR"
echo "Compiler: $CC"
echo "Architecture flags: $ARCH_FLAGS"
echo ""

# Create build directory
BUILD_DIR="$OUTPUT_DIR/build"
mkdir -p "$BUILD_DIR"

cd "$BUILD_DIR"

# Run CMake configuration
if [ "$COMPILER" = "vs" ]; then
    cmake -G "$CMAKE_GENERATOR" \
          -A "$CMAKE_ARCH" \
          -DWRK_ARCH="$ARCH" \
          -DCMAKE_BINARY_DIR="$OUTPUT_DIR" \
          "$WORKSPACE_DIR"
else
    cmake -G "$CMAKE_GENERATOR" \
          -DWRK_ARCH="$ARCH" \
          -DCMAKE_C_COMPILER="$CC" \
          -DCMAKE_CXX_COMPILER="$CXX" \
          -DCMAKE_ASM_FLAGS="$ARCH_FLAGS" \
          -DCMAKE_C_FLAGS="$ARCH_FLAGS" \
          -DCMAKE_CXX_FLAGS="$ARCH_FLAGS" \
          -DCMAKE_BINARY_DIR="$OUTPUT_DIR" \
          "$WORKSPACE_DIR"
fi

echo ""
echo "Building..."
if [ "$COMPILER" = "vs" ]; then
    cmake --build . --config Release
else
    cmake --build . -- -j$(nproc)
fi

echo ""
echo "Build completed successfully!"
echo "Output directory: $OUTPUT_DIR"
echo ""

# Copy HAL stub to output
cp "$WORKSPACE_DIR/hal_stub.c" "$OUTPUT_DIR/"

echo "HAL stub copied to $OUTPUT_DIR/hal_stub.c"
echo ""
echo "To use the built kernel:"
echo "  - The kernel executable is in $OUTPUT_DIR/EXE/"
echo "  - Object files are in $OUTPUT_DIR/obj/$ARCH/"
echo "  - Libraries are in $OUTPUT_DIR/lib/"
