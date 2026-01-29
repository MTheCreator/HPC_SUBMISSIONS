#!/bin/bash
# Fix HPL build on macOS - Complete build version
# This script ensures proper build order

echo "Fixing HPL build for macOS..."

# Get the HPL directory
HPL_DIR=$(pwd)

if [ ! -f "Make.mac" ]; then
    echo "ERROR: Make.mac not found in current directory"
    echo "Please make sure you're in the hpl-2.3 directory and have created Make.mac"
    exit 1
fi

echo "Found Make.mac in: $HPL_DIR"

# First, clean any previous build attempts
echo "Cleaning previous build..."
make arch=mac clean_arch_all 2>/dev/null || true

# Remove the arch-specific directories
rm -rf include/mac lib/mac bin/mac
rm -rf src/*/mac testing/*/mac

# Manually create all necessary directories
echo "Creating directory structure..."

# Main directories
mkdir -p include/mac
mkdir -p lib/mac
mkdir -p bin/mac

# Source directories
mkdir -p src/auxil/mac
mkdir -p src/blas/mac
mkdir -p src/comm/mac
mkdir -p src/grid/mac
mkdir -p src/panel/mac
mkdir -p src/pauxil/mac
mkdir -p src/pfact/mac
mkdir -p src/pgesv/mac

# Testing directories
mkdir -p testing/matgen/mac
mkdir -p testing/timer/mac
mkdir -p testing/pmatgen/mac
mkdir -p testing/ptimer/mac
mkdir -p testing/ptest/mac

echo "Directories created successfully"

# Now create the symbolic links
echo "Creating symbolic links..."

# Source directories
ln -sf "$HPL_DIR/Make.mac" src/auxil/mac/Make.inc
ln -sf "$HPL_DIR/Make.mac" src/blas/mac/Make.inc
ln -sf "$HPL_DIR/Make.mac" src/comm/mac/Make.inc
ln -sf "$HPL_DIR/Make.mac" src/grid/mac/Make.inc
ln -sf "$HPL_DIR/Make.mac" src/panel/mac/Make.inc
ln -sf "$HPL_DIR/Make.mac" src/pauxil/mac/Make.inc
ln -sf "$HPL_DIR/Make.mac" src/pfact/mac/Make.inc
ln -sf "$HPL_DIR/Make.mac" src/pgesv/mac/Make.inc

# Testing directories
ln -sf "$HPL_DIR/Make.mac" testing/matgen/mac/Make.inc
ln -sf "$HPL_DIR/Make.mac" testing/timer/mac/Make.inc
ln -sf "$HPL_DIR/Make.mac" testing/pmatgen/mac/Make.inc
ln -sf "$HPL_DIR/Make.mac" testing/ptimer/mac/Make.inc
ln -sf "$HPL_DIR/Make.mac" testing/ptest/mac/Make.inc

# Verify the links
echo ""
echo "Verifying symbolic links..."
LINKS_OK=true
for dir in src/auxil src/blas src/comm src/grid src/panel src/pauxil src/pfact src/pgesv testing/matgen testing/timer testing/pmatgen testing/ptimer testing/ptest; do
    if [ -f "$dir/mac/Make.inc" ]; then
        echo "✓ $dir/mac/Make.inc"
    else
        echo "✗ Missing: $dir/mac/Make.inc"
        LINKS_OK=false
    fi
done

if [ "$LINKS_OK" = false ]; then
    echo ""
    echo "ERROR: Some symbolic links failed to create"
    exit 1
fi

echo ""
echo "All symbolic links created successfully!"
echo ""

# Copy makefiles
echo "Copying makefiles..."
cp makes/Make.auxil    src/auxil/mac/Makefile
cp makes/Make.blas     src/blas/mac/Makefile
cp makes/Make.comm     src/comm/mac/Makefile
cp makes/Make.grid     src/grid/mac/Makefile
cp makes/Make.panel    src/panel/mac/Makefile
cp makes/Make.pauxil   src/pauxil/mac/Makefile
cp makes/Make.pfact    src/pfact/mac/Makefile
cp makes/Make.pgesv    src/pgesv/mac/Makefile
cp makes/Make.matgen   testing/matgen/mac/Makefile
cp makes/Make.timer    testing/timer/mac/Makefile
cp makes/Make.pmatgen  testing/pmatgen/mac/Makefile
cp makes/Make.ptimer   testing/ptimer/mac/Makefile
cp makes/Make.ptest    testing/ptest/mac/Makefile

echo ""
echo "=========================================="
echo "Building HPL library components..."
echo "=========================================="
echo ""

# Build each component individually to ensure proper order
echo "Building auxiliary routines..."
cd src/auxil/mac && make && cd ../../..

echo "Building BLAS wrappers..."
cd src/blas/mac && make && cd ../../..

echo "Building communication routines..."
cd src/comm/mac && make && cd ../../..

echo "Building grid routines..."
cd src/grid/mac && make && cd ../../..

echo "Building panel routines..."
cd src/panel/mac && make && cd ../../..

echo "Building panel auxiliary routines..."
cd src/pauxil/mac && make && cd ../../..

echo "Building panel factorization..."
cd src/pfact/mac && make && cd ../../..

echo "Building panel solver..."
cd src/pgesv/mac && make && cd ../../..

echo ""
echo "=========================================="
echo "Creating HPL library archive..."
echo "=========================================="
echo ""

# Collect all object files and create the library
cd lib/mac
ar r libhpl.a ../../src/auxil/mac/*.o ../../src/blas/mac/*.o ../../src/comm/mac/*.o ../../src/grid/mac/*.o ../../src/panel/mac/*.o ../../src/pauxil/mac/*.o ../../src/pfact/mac/*.o ../../src/pgesv/mac/*.o
ranlib libhpl.a
cd ../..

if [ -f "lib/mac/libhpl.a" ]; then
    echo "✓ HPL library created successfully: lib/mac/libhpl.a"
else
    echo "✗ ERROR: Failed to create HPL library"
    exit 1
fi

echo ""
echo "=========================================="
echo "Building test components..."
echo "=========================================="
echo ""

echo "Building matrix generation..."
cd testing/matgen/mac && make && cd ../../..

echo "Building timer..."
cd testing/timer/mac && make && cd ../../..

echo "Building parallel matrix generation..."
cd testing/pmatgen/mac && make && cd ../../..

echo "Building parallel timer..."
cd testing/ptimer/mac && make && cd ../../..

echo ""
echo "=========================================="
echo "Building HPL test executable..."
echo "=========================================="
echo ""

cd testing/ptest/mac && make

if [ -f "xhpl" ]; then
    echo ""
    echo "=========================================="
    echo "✓ SUCCESS! HPL built successfully!"
    echo "=========================================="
    echo ""
    echo "Executable location: testing/ptest/mac/xhpl"
    echo ""
    echo "To run HPL:"
    echo "  cd testing/ptest/mac"
    echo "  ./xhpl"
    echo ""
else
    echo ""
    echo "✗ ERROR: Failed to build xhpl executable"
    exit 1
fi