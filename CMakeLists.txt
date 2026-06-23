cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_COMPILER gcc-13 CACHE FILEPATH "C compiler" FORCE)
set(CMAKE_CXX_COMPILER g++-13 CACHE FILEPATH "C++ compiler" FORCE)

project(
        quantib
        VERSION 0.1.0
        DESCRIPTION "Interactive Brokers modern C++ client"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(absl REQUIRED CONFIG)

set(IBKR_DIR "${CMAKE_SOURCE_DIR}/external/IBJts")
set(LOGGER_DIR "${CMAKE_SOURCE_DIR}/external/spdlog/include")
set(NAMEOF_DIR "${CMAKE_SOURCE_DIR}/external/nameof/include")
set(HTTPLIB_DIR "${CMAKE_SOURCE_DIR}/external/cpp-httplib")
set(JSON_DIR "${CMAKE_SOURCE_DIR}/external/json/include")

add_subdirectory(
        "${IBKR_DIR}/source/cppclient/client"
        "${CMAKE_BINARY_DIR}/IBJts-client-build"
)

file(GLOB_RECURSE QUANTIB_SOURCES
        CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE QUANTIB_HEADERS
        CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/include/*.h"
        "${PROJECT_SOURCE_DIR}/include/*.hpp"
)

add_library(quantib
        ${QUANTIB_SOURCES}
        ${QUANTIB_HEADERS}
)

target_include_directories(quantib
        PUBLIC
        "${PROJECT_SOURCE_DIR}/include"
        "${IBKR_DIR}/source/cppclient/client"
        "${IBKR_DIR}/source/cppclient/client/protobufUnix"
        "${LOGGER_DIR}"
        "${NAMEOF_DIR}"
        "${HTTPLIB_DIR}"
        "${JSON_DIR}"
)

target_link_libraries(quantib
        PUBLIC
        twsapi
)

target_compile_features(quantib PUBLIC cxx_std_23)

if(MSVC)
    target_compile_options(quantib PRIVATE /W4)
else()
    target_compile_options(quantib PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wshadow
    )
endif()

enable_testing()

file(GLOB_RECURSE TEST_SOURCES
        CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/test/*.cpp"
)

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME "${TEST_SOURCE}" NAME_WE)

    add_executable(${TEST_NAME}
            "${TEST_SOURCE}"
    )

    target_link_libraries(${TEST_NAME}
            PRIVATE
            quantib
    )

    add_test(
            NAME ${TEST_NAME}
            COMMAND ${TEST_NAME}
    )
endforeach()