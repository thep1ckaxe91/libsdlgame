# Gemini Tracker & Constraints

## Active Constraints
- Get right to the point. Blunt truth, corporate jargon, stack overflow user mindset ("you're absolutely wrong").
- Provide links to official statements for info related to government, laws, scientific papers.
- No branching conversations (one direction only).
- Ask if there is uncertainty. Don't guess from memory.
- Refactoring must be in a separate branch, only merged when verified.
- `GEMINI.md` must NEVER be staged or committed unless explicitly instructed.
- Any PR requesting merge must be pulled with `gh pr checkout` to observe behavior/testing.
- Features must be self-tested using scripts (e.g., ctest).
- No global package installation (use local/venv).
- Others' contribution comments should never be deleted.
- Manual interactions must be pushed to the top so automatic agents can handle the rest.
- Any command that may run forever must be wrapped in `timeout xxx`.
- Focus on the 80% (infrastructure, CI, setup). DO NOT TOUCH the 20% (debugging and implementation). The user will fix compile errors and implement logic.

## Current Tasks
- [x] Initial CMake and GTest setup
- [x] Initial GitHub Actions CI setup
- [x] Basic test setup for TDD
- [x] Migrate dependencies from FetchContent/system packages to Conan
- [x] Cross-platform GitHub Actions CI setup (Linux xvfb, Windows MSVC, macOS)
- [x] Conan cache export CD pipeline (GitHub Releases)

## Context & Architecture Memory

### Build & Package Architecture
- **Language Standard**: C++20, CMake 3.16+, managed via Conan 2 (`conanfile.py`).
- **Core Dependencies**: `sdl/2.32.10`, `sdl_ttf/2.24.0`, `sdl_image/2.8.8`, `sdl_mixer/2.8.1`, `gtest/1.17.0`.
- **Options**: `shared=False`, `fPIC=True`, `sdl/*:wayland=False`, `sdl/*:pulse=False`.
- **Packaging (`conanfile.py`)**: Exports `libsdlgame/0.1.0` as `GameEngine::GameEngine`, includes both `include` and `include/libsdlgame`.

### API & Engine Design Patterns
- **Templated Surface**: `Surface<SDL_TextureAccess AccessPattern = SDL_TEXTUREACCESS_TARGET>` declared in `include/surface.hpp`. Cross-access operations utilize `friend class Surface;`.
- **Templated Transformations**: `include/transform.hpp` operates across arbitrary source and destination access patterns.
- **Unified Error Handling**: Defined in `include/engine.hpp` via `SDL_CHECK(expr)` (non-negative check) and `SDL_NEW(ptr_expr)` (non-null check). Failures output `"FATAL: SDL Error at <file>:<line>"` to `std::cerr` followed by `std::terminate()`.

### Testing & GTest Death Test Constraints
- **GTest Death Tests**: Must expect regex `"FATAL: SDL Error"` (replaces legacy `"Failed to create texture"`).
- **Process Isolation on Wayland/X11**: Death tests MUST initialize SDL (`sdlgame::init()`) *inside* the `EXPECT_DEATH` block within independent suites (`TEST(*DeathTest, ...)`). Initializing in parent fixtures causes child `sdlgame::quit()` to sever parent display sockets, freezing the desktop compositor.
- **Preprocessor Comma Safety**: Code blocks containing commas (e.g. initializer lists) passed to `EXPECT_DEATH` must be wrapped inside a lambda (`auto test_func = [](){...}; EXPECT_DEATH(test_func(), ...)`).

### CI/CD Pipelines (`.github/workflows/`)
- **`ci.yml` (Matrix: Ubuntu, Windows, macOS)**:
  - **Linux**: Installs `xvfb`, runs with `xvfb-run -a ctest`. Conan invokes `-c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True`.
  - **Windows & macOS**: Native runner GUI contexts used (NO `SDL_VIDEODRIVER: dummy`, as dummy drivers lack `SDL_RENDERER_ACCELERATED` support). Conan relies on auto-detected toolchains (no hardcoded `compiler.version=13`).
- **`cd.yml` (Path 2: Zero-Host Conan Cache Deployment)**:
  - Triggers on tag `v*.*.*`.
  - Runs `conan create . -s build_type=Release` on all 3 OSes.
  - Bundles cache entries via `conan cache save "libsdlgame/0.1.0" --file libsdlgame-<OS>-conan.tgz`.
  - Publishes `.tgz` bundles to GitHub Releases.
  - Downstream consumers restore cache with `conan cache restore <file>.tgz`.
