cmake_minimum_required(VERSION 3.16)

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

# =========================
# IBKR TWS API
# =========================

set(IBKR_DIR "${CMAKE_SOURCE_DIR}/external/IBJts")

add_subdirectory(
    "${IBKR_DIR}/source/cppclient/client"
    "${CMAKE_BINARY_DIR}/IBJts-client-build"
)

# =========================
# QuantIB library
# =========================

add_library(quantib
    src/dummy.cpp
)

target_include_directories(quantib
    PUBLIC
        "${PROJECT_SOURCE_DIR}/include"
        "${IBKR_DIR}/source/cppclient/client"
        "${IBKR_DIR}/source/cppclient/client/protobufUnix"
)

target_link_libraries(quantib
    PUBLIC
        twsapi
)

target_compile_features(quantib PUBLIC cxx_std_23)

# =========================
# Compiler warnings
# =========================

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

# =========================
# Tests
# =========================

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
