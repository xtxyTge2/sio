cmake_minimum_required(VERSION 3.23)

set(PROJECT "sio_include")
project("${PROJECT}")

add_executable("${PROJECT}")
target_sources("${PROJECT}"
    PRIVATE
        main.c
)

target_link_libraries("${PROJECT}" PRIVATE sio)
