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
