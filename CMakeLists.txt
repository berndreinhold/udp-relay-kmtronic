cmake_minimum_required(VERSION 3.13)

project(udp-relay-kmtronic LANGUAGES CXX)

# set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# place binaries and libraries according to GNU standards
include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

set(SOURCES
    udp-relay-kmtronic.cpp
    udp_comm.cpp
    udp_comm_validation.cpp
)

add_executable(udp-relay-kmtronic ${SOURCES})

target_compile_options(udp-relay-kmtronic PUBLIC -O3 -Wall)
target_include_directories(
    udp-relay-kmtronic
    PUBLIC
)
