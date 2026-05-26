cmake_minimum_required(VERSION 3.5)

project(
    quantib
    VERSION 0.1.0
    DESCRIPTION "Interactive Brokers modern C++ client"
    LANGUAGES CXX
)

# =========================
# C++ Standard
# =========================

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# =========================
# Dependencies
# =========================

# Builds the official IBKR TWS C++ API.
# Expected path: external/IBKR/CMakeLists.txt
add_subdirectory(external/IBKR/source/cppclient/client)

# =========================
# Library
# =========================

add_library(quantib
    src/dummy.cpp
)

target_include_directories(quantib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/external/IBKR/source/cppclient/client
)

target_link_libraries(quantib
    PUBLIC
        twsapi
)

target_compile_features(quantib PUBLIC cxx_std_23)

# =========================
# Compiler Warnings
# =========================

if (MSVC)
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
    ${PROJECT_SOURCE_DIR}/test/*.cpp
)

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)

    add_executable(${TEST_NAME}
        ${TEST_SOURCE}
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
