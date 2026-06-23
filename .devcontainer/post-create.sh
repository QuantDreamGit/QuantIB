#!/usr/bin/env bash
set -e

mkdir -p /workspace/external

[ -d /workspace/external/nameof ] || \
  git clone --branch v0.10.4 --depth 1 https://github.com/Neargye/nameof.git /workspace/external/nameof

[ -d /workspace/external/spdlog ] || \
  git clone --branch v1.14.1 --depth 1 https://github.com/gabime/spdlog.git /workspace/external/spdlog

[ -d /workspace/external/cpp-httplib ] || \
  git clone --branch v0.18.6 --depth 1 https://github.com/yhirose/cpp-httplib.git /workspace/external/cpp-httplib

[ -d /workspace/external/json ] || \
  git clone --branch v3.12.0 --depth 1 https://github.com/nlohmann/json.git /workspace/external/json

PROTO_DIR=/workspace/external/IBJts/source/proto
OUT_DIR=/workspace/external/IBJts/source/cppclient/client/protobufUnix
STAMP_FILE="$OUT_DIR/.generated-by-protoc-$(protoc --version | awk '{print $2}')"

if [ ! -f "$STAMP_FILE" ]; then
    mkdir -p "$OUT_DIR"
    rm -f "$OUT_DIR"/*.pb.h "$OUT_DIR"/*.pb.cc "$OUT_DIR"/.generated-by-protoc-*

    protoc \
        --cpp_out="$OUT_DIR" \
        --proto_path="$PROTO_DIR" \
        "$PROTO_DIR"/*.proto

    touch "$STAMP_FILE"
fi