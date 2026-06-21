FROM --platform=linux/amd64 ubuntu:22.04 AS dev

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    git cmake g++ gcc make ninja-build gdb \
    wget curl unzip zlib1g-dev \
    libcurl4-openssl-dev libeigen3-dev \
    pkg-config ca-certificates \
    clang clang-format clang-tidy \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get purge -y 'libabsl*' || true

RUN echo "/usr/local/lib" > /etc/ld.so.conf.d/usr-local.conf && ldconfig

ARG ABSL_VERSION=20240722.0
RUN git clone --branch ${ABSL_VERSION} --depth 1 https://github.com/abseil/abseil-cpp.git /tmp/abseil && \
    cd /tmp/abseil && \
    cmake -S . -B build \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
      -DCMAKE_INSTALL_PREFIX=/usr/local && \
    cmake --build build -j$(nproc) && \
    cmake --install build && \
    rm -rf /tmp/abseil && \
    ldconfig

ARG PROTOBUF_VERSION=v31.1
RUN git clone --branch ${PROTOBUF_VERSION} --depth 1 https://github.com/protocolbuffers/protobuf.git /tmp/protobuf && \
    cd /tmp/protobuf && \
    cmake -S . -B build \
      -Dprotobuf_BUILD_TESTS=OFF \
      -Dprotobuf_BUILD_SHARED_LIBS=ON \
      -Dprotobuf_USE_EXTERNAL_GTEST=OFF \
      -Dprotobuf_ABSL_PROVIDER=module \
      -Dprotobuf_WITH_ZLIB=ON \
      -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/usr/local && \
    cmake --build build -j$(nproc) && \
    cmake --install build && \
    rm -rf /tmp/protobuf && \
    ldconfig

RUN cd /tmp && \
    wget http://www.netlib.org/misc/intel/IntelRDFPMathLib20U2.tar.gz && \
    tar -xzf IntelRDFPMathLib20U2.tar.gz && \
    cd IntelRDFPMathLib20U2/LIBRARY && \
    make CC=gcc CALL_BY_REF=0 GLOBAL_RND=0 GLOBAL_FLAGS=0 UNCHANGED_BINARY_FLAGS=0 && \
    cp libbid.a /usr/local/lib/ && \
    mkdir -p /usr/local/include/libbid && \
    cp -r ../LIBRARY/src /usr/local/include/libbid/ && \
    rm -rf /tmp/IntelRDFPMathLib20U2* && \
    ldconfig

WORKDIR /workspace