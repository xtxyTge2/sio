all: (build "asan") test_all

build preset:
    cmake --preset {{preset}}
    cmake --build build/{{preset}}

test preset: (build preset)
    ctest --test-dir build/{{preset}} -V

test_all: (test "debug") (test "asan") (test "ubsan") (test "debug_no_uring") (test "asan_no_uring") (test "ubsan_no_uring")

gdb:
	gdb --args ./build/debug/examples/01_sio_include/sio_include ./build/examples/01_sio_include/input.txt

run:
	./build/debug/examples/01_sio_include/sio_include examples/01_sio_include/input.txt

clean:
    rm -rf ./build
