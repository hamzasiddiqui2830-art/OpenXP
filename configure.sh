#!/bin/bash
# OpenXP configure/build entry point.
# The compiler is detected from the environment when no compiler is supplied.
# Usage: ./configure.sh [compiler] [architecture]
#   compiler: auto, mingw, gcc, clang, vs, llvm
#   architecture: auto, i386, amd64

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKSPACE_DIR="$SCRIPT_DIR"

COMPILER="${1:-auto}"
ARCH="${2:-auto}"

if [ "$COMPILER" = "auto" ]; then
    if command -v cl.exe >/dev/null 2>&1 || command -v cl >/dev/null 2>&1 || [ -n "${VSCMD_VER:-}" ] || [ -n "${VSINSTALLDIR:-}" ]; then
        COMPILER="vs"
    elif command -v clang-cl.exe >/dev/null 2>&1 || command -v clang-cl >/dev/null 2>&1; then
        COMPILER="clang"
    elif command -v x86_64-w64-mingw32-gcc >/dev/null 2>&1 || command -v i686-w64-mingw32-gcc >/dev/null 2>&1; then
        COMPILER="mingw"
    elif command -v gcc >/dev/null 2>&1; then
        COMPILER="gcc"
    elif command -v clang >/dev/null 2>&1; then
        COMPILER="clang"
    else
        echo "Error: could not detect a supported compiler."
        echo "Run from a Visual Studio Developer Command Prompt, or pass a compiler explicitly."
        echo "Supported compilers: mingw, gcc, clang, vs, llvm"
        exit 1
    fi
fi

case "$COMPILER" in
    mingw|gcc|clang|vs|llvm) ;;
    *) echo "Error: unsupported compiler '$COMPILER'"; exit 1 ;;
esac

if [ "$ARCH" = "auto" ]; then
    case "${PROCESSOR_ARCHITEW6432:-${PROCESSOR_ARCHITECTURE:-}}" in
        AMD64|IA64|ARM64|x86_64|aarch64) ARCH="amd64" ;;
        *)
            if [ "$(uname -m 2>/dev/null || true)" = "x86_64" ]; then ARCH="amd64"; else ARCH="i386"; fi
            ;;
    esac
fi

case "$ARCH" in
    i386|amd64) ;;
    *) echo "Error: unsupported architecture '$ARCH'"; exit 1 ;;
esac

OUTPUT_DIR="$WORKSPACE_DIR/output-${COMPILER}-${ARCH}"
BUILD_DIR="$OUTPUT_DIR/build"

case "$COMPILER" in
    mingw)
        if [ "$ARCH" = "i386" ]; then
            CC="i686-w64-mingw32-gcc"; CXX="i686-w64-mingw32-g++"; ARCH_FLAGS="-m32"
        else
            CC="x86_64-w64-mingw32-gcc"; CXX="x86_64-w64-mingw32-g++"; ARCH_FLAGS="-m64"
        fi
        GENERATOR="Ninja"
        ;;
    gcc)
        CC="gcc"; CXX="g++"; GENERATOR="Ninja"
        [ "$ARCH" = "i386" ] && ARCH_FLAGS="-m32" || ARCH_FLAGS="-m64"
        ;;
    clang)
        if command -v clang-cl.exe >/dev/null 2>&1 || command -v clang-cl >/dev/null 2>&1; then
            CC="clang-cl"; CXX="clang-cl"; GENERATOR="Ninja"; ARCH_FLAGS=""
        else
            CC="clang"; CXX="clang++"; GENERATOR="Ninja"
            [ "$ARCH" = "i386" ] && ARCH_FLAGS="-m32" || ARCH_FLAGS="-m64"
        fi
        ;;
    llvm)
        CC="clang"; CXX="clang++"; GENERATOR="Ninja"
        [ "$ARCH" = "i386" ] && ARCH_FLAGS="-m32" || ARCH_FLAGS="-m64"
        ;;
    vs)
        CC="cl"; CXX="cl"; GENERATOR="Ninja"; ARCH_FLAGS=""
        ;;
esac

if ! command -v cmake >/dev/null 2>&1; then
    echo "Error: cmake was not found in PATH."; exit 1
fi
if ! command -v ninja >/dev/null 2>&1; then
    echo "Error: ninja was not found in PATH."; exit 1
fi

rm -rf "$OUTPUT_DIR"
mkdir -p "$BUILD_DIR"

echo "=============================================="
echo "OpenXP configure/build"
echo "=============================================="
echo "Compiler:     $COMPILER"
echo "Architecture: $ARCH"
echo "Generator:    $GENERATOR"
echo "Output:       $OUTPUT_DIR"
echo "=============================================="

export CC CXX

if [ "$COMPILER" = "vs" ]; then
    cmake -S "$WORKSPACE_DIR" -B "$BUILD_DIR" -G "$GENERATOR" \
        -DWRK_ARCH="$ARCH" -DCMAKE_BUILD_TYPE=Release
else
    cmake -S "$WORKSPACE_DIR" -B "$BUILD_DIR" -G "$GENERATOR" \
        -DWRK_ARCH="$ARCH" \
        -DCMAKE_C_COMPILER="$CC" -DCMAKE_CXX_COMPILER="$CXX" \
        -DCMAKE_C_FLAGS="$ARCH_FLAGS" -DCMAKE_CXX_FLAGS="$ARCH_FLAGS" \
        -DCMAKE_BUILD_TYPE=Release
fi

cmake --build "$BUILD_DIR" --parallel

echo ""
echo "Build completed successfully."
echo "Output directory: $OUTPUT_DIR"
