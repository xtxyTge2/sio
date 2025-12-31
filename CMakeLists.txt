cmake_minimum_required(VERSION 3.23)
project(simple_io_library)

add_library(sio)
target_sources(sio
    PRIVATE
        src/sio_linux.c
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/sio/sio.h
)
add_subdirectory(examples/01_sio_include)
add_subdirectory(external/Unity)
add_subdirectory(test)

add_test(NAME sio_linux COMMAND test_sio_linux)
enable_testing()
