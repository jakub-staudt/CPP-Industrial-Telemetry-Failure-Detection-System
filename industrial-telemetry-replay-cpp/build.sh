#!/bin/bash

# Industrial Telemetry Build Script

set -e

echo "╔═══════════════════════════════════════════════════════════╗"
echo "║  Industrial Telemetry Replay - Build Script              ║"
echo "╚═══════════════════════════════════════════════════════════╝"
echo ""

# Check if CMake is available
if ! command -v cmake &> /dev/null; then
    echo "❌ Error: CMake is not installed. Please install CMake 3.16 or later."
    exit 1
fi

# Check if C++ compiler is available
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo "❌ Error: No C++ compiler found. Please install g++ or clang."
    exit 1
fi

# Create build directory
if [ ! -d "build" ]; then
    echo "📁 Creating build directory..."
    mkdir -p build
fi

# Run CMake
echo "🔨 Configuring with CMake..."
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
echo "⚙️  Compiling..."
make -j$(nproc)

echo ""
echo "✅ Build successful!"
echo ""
echo "📦 Executable location: ./industrial_monitor"
echo ""
echo "Usage example:"
echo "  ./industrial_monitor ../data/ai4i2020.csv --limit 300 --delay 50"
echo ""
