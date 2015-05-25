#Authors:
# Giorgi Dundua
# Saimonas Sileikis

cmake_minimum_required(VERSION 2.8)
project(PROXY)

set(serial_SRCS include/serial/serial.cc include/serial/serial.h include/serial/v8stdint.h include/serial/unix.cc)
set(NANOPB_SRC_ROOT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/include/nanopb)
set(CMAKE_MODULE_PATH ${NANOPB_SRC_ROOT_FOLDER}/extra)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -g -O0")

set_source_files_properties(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES GENERATED TRUE)

find_package(Nanopb REQUIRED)

nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS message.proto)
include_directories(include ${NANOPB_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})

add_library(serial ${serial_SRCS})
target_link_libraries(serial rt)
add_executable(proxy proxy.cpp ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(proxy serial)
