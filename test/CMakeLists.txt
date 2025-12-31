add_executable(test_sio_linux)

target_include_directories(test_sio_linux PRIVATE ../external/Unity/src)
target_sources(test_sio_linux
    PRIVATE
        test_sio_linux.c
)

target_compile_options(test_sio_linux PRIVATE -Wall -Wpedantic -Werror -Wshadow)
target_link_libraries(test_sio_linux PRIVATE sio unity)

add_test(NAME sio_linux COMMAND test_sio_linux)
