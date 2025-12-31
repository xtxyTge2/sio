cmake_minimum_required(VERSION 3.23)
project(simple_io_library)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

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

option(SIO_USE_URING "Use io_uring backend" OFF)
if(SIO_USE_URING)
    target_compile_definitions(sio PRIVATE SIO_USE_URING)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(URING REQUIRED liburing)
    target_link_libraries(sio ${URING_LIBRARIES})
    target_include_directories(sio PRIVATE ${URING_INCLUDE_DIRS})
endif()

target_compile_options(sio PRIVATE -Wall -Wpedantic -Werror -Wshadow)

add_subdirectory(examples/01_sio_include)
add_subdirectory(external/Unity)
add_subdirectory(test)

add_test(NAME sio_linux COMMAND test_sio_linux)
enable_testing()
