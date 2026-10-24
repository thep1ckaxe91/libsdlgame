.PHONY: help setup-debug setup-release setup-relwithdebinfo \
				debug release relwithdebinfo \
				test gdb perf \
				package editable editable-remove clean format

# Configuration
BUILD_DIR ?= build
TEST ?= .*
PARALLEL ?= $(shell nproc 2>/dev/null || echo 4)

help:
		@echo "libsdlgame developer workflow targets:"
		@echo "  make debug             - Build Debug target (-O0 -g3)"
		@echo "  make release           - Build Release target (-O3)"
		@echo "  make relwithdebinfo    - Build RelWithDebInfo target (-O3 -g for profiling)"
		@echo "  make test [TEST=regex] - Run tests via ctest (e.g. make test TEST=ColorTest)"
		@echo "  make gdb TEST=<name>   - Run a test executable directly under gdb"
		@echo "  make perf TEST=<name>  - Run Linux perf record on a RelWithDebInfo test"
		@echo "  make package           - Run conan create into local cache"
		@echo "  make editable          - Register live source tree into local Conan"
		@echo "  make editable-remove   - Unregister live source tree from Conan"
		@echo "  make clean             - Remove all build directories"

# -----------------------------------------------------------------------------
# 1. Setup & Configuration (Conan + CMake presets)
# -----------------------------------------------------------------------------
setup-debug:
		conan install . --build=missing -s build_type=Debug
		cmake --preset conan-debug
		ln -sf $(BUILD_DIR)/Debug/compile_commands.json compile_commands.json

setup-release:
		conan install . --build=missing -s build_type=Release
		cmake --preset conan-release

setup-relwithdebinfo:
		conan install . --build=missing -s build_type=RelWithDebInfo
		cmake --preset conan-relwithdebinfo
		ln -sf $(BUILD_DIR)/RelWithDebInfo/compile_commands.json compile_commands.json

# -----------------------------------------------------------------------------
# 2. Compilation
# -----------------------------------------------------------------------------
debug: setup-debug
		cmake --build --preset conan-debug -j$(PARALLEL)

release: setup-release
		cmake --build --preset conan-release -j$(PARALLEL)

relwithdebinfo: setup-relwithdebinfo
		cmake --build --preset conan-relwithdebinfo -j$(PARALLEL)

# -----------------------------------------------------------------------------
# 3. Testing, Debugging & Profiling
# -----------------------------------------------------------------------------
test: debug
		SDL_AUDIODRIVER=dummy ctest --test-dir $(BUILD_DIR)/Debug --output-on-failure -R "$(TEST)"

# Quickly launch a failed test inside GDB (e.g., make gdb TEST=test_draw)
gdb: debug
		SDL_AUDIODRIVER=dummy gdb --args $(BUILD_DIR)/Debug/tests/$(TEST)

# Profile optimized code with symbols using Linux perf
perf: relwithdebinfo
		@echo "Recording CPU profile for tests/$(TEST)..."
		SDL_AUDIODRIVER=dummy perf record -F 999 -g -- $(BUILD_DIR)/RelWithDebInfo/tests/$(TEST)
		perf report

# -----------------------------------------------------------------------------
# 4. Packaging & Consumption (Conan)
# -----------------------------------------------------------------------------
package:
		conan create . --build=missing -s build_type=Release

# Live editable mode: your main project consumes libsdlgame without re-packaging
editable:
		conan editable add .

editable-remove:
		conan editable remove libsdlgame/0.1.0

# -----------------------------------------------------------------------------
# 5. Housekeeping
# -----------------------------------------------------------------------------
clean:
		rm -rf $(BUILD_DIR)

format:
		find src include tests -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i