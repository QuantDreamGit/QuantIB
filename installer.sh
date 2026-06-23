#!/usr/bin/env bash

set -euo pipefail

echo "Installing dependencies..."

sudo apt update

sudo apt install -y \
    git cmake g++ gcc make ninja-build gdb \
    wget curl unzip zlib1g-dev \
    libcurl4-openssl-dev libeigen3-dev \
    pkg-config ca-certificates \
    clang clang-format clang-tidy tzdata

echo "Installing GCC/G++ 13..."

sudo apt update
sudo apt install -y gcc-13 g++-13

sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100

echo "Building Abseil..."

cd /tmp

rm -rf abseil-cpp

git clone \
    --branch 20240722.0 \
    --depth 1 \
    https://github.com/abseil/abseil-cpp.git

cd abseil-cpp

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    -DCMAKE_INSTALL_PREFIX=/usr/local

cmake --build build -j"$(nproc)"
sudo cmake --install build
sudo ldconfig

echo "Building Protobuf..."

cd /tmp

rm -rf protobuf

git clone \
    --branch v31.1 \
    --recurse-submodules \
    --depth 1 \
    https://github.com/protocolbuffers/protobuf.git

cd protobuf

cmake -S . -B build \
    -Dprotobuf_BUILD_TESTS=OFF \
    -Dprotobuf_BUILD_SHARED_LIBS=ON \
    -Dprotobuf_ABSL_PROVIDER=module \
    -Dprotobuf_WITH_ZLIB=ON \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local

cmake --build build -j"$(nproc)"
sudo cmake --install build
sudo ldconfig

echo "Building Intel Decimal Library..."

cd /tmp

rm -rf IntelRDFPMathLib20U2*

wget http://www.netlib.org/misc/intel/IntelRDFPMathLib20U2.tar.gz

tar -xzf IntelRDFPMathLib20U2.tar.gz

cd IntelRDFPMathLib20U2/LIBRARY

make \
    CC=gcc \
    CFLAGS="-fPIC" \
    CALL_BY_REF=0 \
    GLOBAL_RND=0 \
    GLOBAL_FLAGS=0 \
    UNCHANGED_BINARY_FLAGS=0

sudo cp libbid.a /usr/local/lib/

sudo mkdir -p /usr/local/include/libbid
sudo cp -r ../LIBRARY/src /usr/local/include/libbid/

sudo ldconfig

echo "Done."

echo
echo "Compiler versions:"
gcc-13 --version | head -n 1
g++-13 --version | head -n 1
protoc --version || true