file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

file(GLOB PROTO_SOURCES "protobufUnix/*.pb.cc")
file(GLOB PROTO_HEADERS "protobufUnix/*.pb.h")

set(twsapi_VERSION 9.79.02)

find_library(BID_LIBRARY
        NAMES bid libbid
        PATHS /usr/local/lib
        REQUIRED
)

find_package(Protobuf REQUIRED CONFIG)
find_package(absl REQUIRED CONFIG)

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
        absl::log
        absl::check
        absl::hash
        absl::strings
        ${BID_LIBRARY}
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