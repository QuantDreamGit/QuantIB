
````markdown
# IBKR TWS API C++ Setup with CMake

This project uses the Interactive Brokers TWS API C++ client located in:

```text
external/IBJts
````

The project expects this structure:

```text
QuantIB/
  CMakeLists.txt
  src/
    main.cpp
  external/
    IBJts/
      source/
        cppclient/
          client/
            CMakeLists.txt
        proto/
```

## 1. Install system dependencies

On Ubuntu/Linux:

```bash
sudo apt update
sudo apt install -y build-essential cmake ninja-build protobuf-compiler libprotobuf-dev
```

Check that protobuf is installed:

```bash
protoc --version
```

## 2. Generate IBKR protobuf C++ files

From the project root:

```bash
cd external/IBJts/source

mkdir -p cppclient/client/protobufUnix

protoc \
  --proto_path=./proto \
  --experimental_allow_proto3_optional \
  --cpp_out=./cppclient/client/protobufUnix \
  proto/*.proto
```

Verify that files were generated:

```bash
ls cppclient/client/protobufUnix
```

You should see files such as:

```text
ExecutionRequest.pb.h
ExecutionRequest.pb.cc
ExecutionDetails.pb.h
ExecutionDetails.pb.cc
Order.pb.h
Order.pb.cc
```

## 3. Patch `external/IBJts/source/cppclient/client/CMakeLists.txt`

Replace the IBKR client CMake file with this minimal version:

```cmake
file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

file(GLOB PROTO_SOURCES "protobufUnix/*.pb.cc")
file(GLOB PROTO_HEADERS "protobufUnix/*.pb.h")

set(twsapi_VERSION 9.79.02)

find_package(Protobuf REQUIRED)

add_library(twsapi SHARED
    ${SOURCES}
    ${PROTO_SOURCES}
)

target_include_directories(twsapi
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/protobufUnix
)

target_link_libraries(twsapi
    PUBLIC
        protobuf::libprotobuf
)

if(WIN32)
    target_link_libraries(twsapi PUBLIC ws2_32)
endif()

if(MSVC)
    target_compile_options(twsapi PRIVATE /W3)
else()
    target_compile_options(twsapi PRIVATE -Wall -Wextra -pedantic -Wno-unused-parameter)
endif()

set_target_properties(twsapi
    PROPERTIES
    PREFIX "lib"
    DEBUG_POSTFIX "d"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    PUBLIC_HEADER "${HEADERS};${PROTO_HEADERS}"
)

install(
    TARGETS twsapi
    EXPORT twsapi-targets
    PUBLIC_HEADER DESTINATION include/twsapi
    LIBRARY DESTINATION lib
)
```

This modification makes the `twsapi` target compile the generated protobuf sources and expose the correct include directories.

## 4. Root `CMakeLists.txt`

Create or update the root `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.16)

project(QuantIB LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(IBJTS_DIR "${CMAKE_SOURCE_DIR}/external/IBJts")

add_subdirectory(
    "${IBJTS_DIR}/source/cppclient/client"
    "${CMAKE_BINARY_DIR}/IBJts-client-build"
)

add_executable(my_ibkr_app
    src/main.cpp
)

target_link_libraries(my_ibkr_app
    PRIVATE
        twsapi
)
```

## 5. Minimal `src/main.cpp`

```cpp
#include <iostream>

#include "EWrapper.h"
#include "EClientSocket.h"

int main() {
    std::cout << "IBKR TWS API linked successfully" << std::endl;
    return 0;
}
```

## 6. Build

Always build from the project root:

```bash
cd /home/quantdream/Documents/QuantIB

rm -rf build

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug

cmake --build build
```

The executable will be built under:

```text
build/
```

The TWS API shared library will be generated under:

```text
build/lib/
```

## Common issues

### `ExecutionRequest.pb.h: No such file or directory`

The protobuf files were not generated or the `protobufUnix` directory is not included by CMake.

Run again:

```bash
cd external/IBJts/source

protoc \
  --proto_path=./proto \
  --experimental_allow_proto3_optional \
  --cpp_out=./cppclient/client/protobufUnix \
  proto/*.proto
```

Then rebuild from the project root.

### `external/IBJts/source does not contain CMakeLists.txt`

Do not run CMake from `external/IBJts/source`.

Run CMake only from the project root:

```bash
cd /home/quantdream/Documents/QuantIB
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

### Top-level `external/IBJts/CMakeLists.txt` fails

Do not add the whole `external/IBJts` directory as a CMake subdirectory.

Use only:

```cmake
add_subdirectory(
    "${IBJTS_DIR}/source/cppclient/client"
    "${CMAKE_BINARY_DIR}/IBJts-client-build"
)
```

```

This matches your generated `protobufUnix` layout and the current IBKR client CMake structure you showed.
```
