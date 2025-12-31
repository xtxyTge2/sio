all: build test

build_debug:
	mkdir -p build
	cmake -S . -B build/debug --preset debug
	cmake --build build/debug

build_asan:
    mkdir -p build
    cmake -S . -B build/asan --preset asan
    cmake --build build/asan

build_ubsan:
    mkdir -p build
    cmake -S . -B build/ubsan --preset ubsan
    cmake --build build/ubsan

test_debug: build_debug
    ctest --test-dir build/debug --rerun-failed --output-on-failure

build: build_debug
test: build test_debug

gdb:
	gdb --args ./build/debug/examples/01_sio_include/sio_include ./build/examples/01_sio_include/input.txt

run:
	./build/debug/examples/01_sio_include/sio_include examples/01_sio_include/input.txt

clean:
    rm -rf ./build
