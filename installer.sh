#!/usr/bin/env bash

set -euo pipefail

PROJECT_DIR="${PROJECT_DIR:-$HOME/CLionProjects/QuantIB}"
EXTERNAL_DIR="$PROJECT_DIR/external"
MARKER_DIR="$HOME/.local/state/quantib-setup"

ABSL_VERSION="20240722.0"
PROTOBUF_VERSION="v31.1"

mkdir -p "$MARKER_DIR"

update_or_clone() {
    local repo="$1"
    local dir="$2"

    if [ -d "$dir/.git" ]; then
        echo "Updating $(basename "$dir")..."
        git -C "$dir" pull --ff-only
    else
        echo "Cloning $(basename "$dir")..."
        git clone --depth 1 "$repo" "$dir"
    fi
}

echo "Installing apt dependencies..."

sudo apt update
sudo apt install -y \
    git cmake g++ gcc make ninja-build gdb \
    wget curl unzip zlib1g-dev \
    libcurl4-openssl-dev libeigen3-dev \
    pkg-config ca-certificates \
    clang clang-format clang-tidy tzdata \
    gcc-13 g++-13

sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100

if [ ! -f "$MARKER_DIR/abseil-$ABSL_VERSION.done" ]; then
    echo "Building Abseil..."

    cd /tmp
    rm -rf abseil-cpp

    git clone --branch "$ABSL_VERSION" --depth 1 https://github.com/abseil/abseil-cpp.git

    cd abseil-cpp

    cmake -S . -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
        -DCMAKE_INSTALL_PREFIX=/usr/local

    cmake --build build -j"$(nproc)"
    sudo cmake --install build
    sudo ldconfig

    touch "$MARKER_DIR/abseil-$ABSL_VERSION.done"
else
    echo "Abseil already installed, skipping."
fi

if [ ! -f "$MARKER_DIR/protobuf-$PROTOBUF_VERSION.done" ]; then
    echo "Building Protobuf..."

    cd /tmp
    rm -rf protobuf

    git clone --branch "$PROTOBUF_VERSION" --recurse-submodules --depth 1 https://github.com/protocolbuffers/protobuf.git

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

    touch "$MARKER_DIR/protobuf-$PROTOBUF_VERSION.done"
else
    echo "Protobuf already installed, skipping."
fi

if [ ! -f "$MARKER_DIR/libbid.done" ]; then
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

    touch "$MARKER_DIR/libbid.done"
else
    echo "Intel Decimal Library already installed, skipping."
fi

echo "Fetching project external dependencies..."

mkdir -p "$EXTERNAL_DIR"

update_or_clone https://github.com/Neargye/nameof.git "$EXTERNAL_DIR/nameof"
update_or_clone https://github.com/gabime/spdlog.git "$EXTERNAL_DIR/spdlog"
update_or_clone https://github.com/yhirose/cpp-httplib.git "$EXTERNAL_DIR/cpp-httplib"
update_or_clone https://github.com/nlohmann/json.git "$EXTERNAL_DIR/json"

echo "Generating protobuf files if needed..."

PROTO_DIR="$EXTERNAL_DIR/IBJts/source/proto"
OUT_DIR="$EXTERNAL_DIR/IBJts/source/cppclient/client/protobufUnix"
PROTOC_VERSION="$(protoc --version | awk '{print $2}')"
STAMP_FILE="$OUT_DIR/.generated-by-protoc-$PROTOC_VERSION"

if [ ! -d "$PROTO_DIR" ]; then
    echo "ERROR: Proto directory not found: $PROTO_DIR"
    echo "Make sure external/IBJts exists in your project."
    exit 1
fi

if [ ! -f "$STAMP_FILE" ]; then
    mkdir -p "$OUT_DIR"
    rm -f "$OUT_DIR"/*.pb.h "$OUT_DIR"/*.pb.cc "$OUT_DIR"/.generated-by-protoc-*

    protoc \
        --cpp_out="$OUT_DIR" \
        --proto_path="$PROTO_DIR" \
        "$PROTO_DIR"/*.proto

    touch "$STAMP_FILE"
else
    echo "Protobuf files already generated with protoc $PROTOC_VERSION, skipping."
fi

echo "Done."

echo
echo "Compiler versions:"
gcc-13 --version | head -n 1
g++-13 --version | head -n 1
protoc --version || true