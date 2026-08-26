.PHONY: install build test clean

TEST_SUITE ?= .*

install:
	conan install . --build=missing -s build_type=Debug -s:b compiler.version=13 -s:b compiler.cppstd=20

build: install
	cmake --preset conan-debug
	cmake --build --preset conan-debug -j$(nproc)

test: build
	ctest --test-dir build/Debug --output-on-failure -R "$(TEST_SUITE)"

clean:
	rm -rf build

release: 
	conan install . --build=missing -s build_type=Release -s:b compiler.version=13 -s:b compiler.cppstd=20
	cmake --preset conan-release
	cmake --build --preset conan-release -j$(nproc)
	