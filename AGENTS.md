# Agent Guidelines for algorithms4th

This repository is a small C++ algorithm-practice library. It is separate from
the parent `/home/xiaodong/rrison` tree and has its own `.git` directory.

## Repository Layout

- Public headers live under `include/`.
  - General algorithms: `include/algorithms4th/*.hpp`
  - DSP utilities: `include/dsp/*.hpp`
- Tests live under `tests/*.cpp`.
- The project is header-only and exposed through the `algorithms4th` CMake
  `INTERFACE` target.
- Build artifacts should stay under `build/`, which is gitignored.

## Build and Test

Use CMake and CTest from the repository root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

When adding a new header, add or update a focused test in `tests/` and wire it
into `CMakeLists.txt` with `add_executable(...)`, `target_link_libraries(...)`,
and `add_test(...)`.

## Coding Style

- Keep implementations simple and educational.
- Prefer header-only templates for generic algorithms.
- Use `#pragma once` in headers.
- Do not put `using namespace std;` in headers.
- Avoid global `using std::...` declarations in headers. Prefer `std::vector`,
  `std::size_t`, etc. A narrow local `using std::swap;` inside a swap helper is
  fine.
- Prefer half-open ranges `[left, right)` for recursive and iterative sorting
  helpers.
- Keep algorithm headers free of benchmark output or other side effects.
  Put timing code in tests or dedicated benchmark programs.
- For generic sorting code, prefer comparisons based on `<` only when practical,
  rather than requiring both `<` and `<=`.

## Testing Expectations

- Tests may use plain `assert`.
- For sorting tests, compare against `std::sort` on deterministic random data
  where useful.
- Benchmark prints are acceptable in tests, but correctness assertions should
  not depend on timing.
- For thread-aware utilities such as `Conv1dStream`, include a minimal
  concurrency smoke test and link `Threads::Threads`.

## Collaboration Style

This repository is used for learning algorithms. If the user asks for a concept
or says they want to write the code themselves, explain the principle and review
their attempt instead of jumping to a full implementation. If the user asks for
an implementation or a test, make the scoped change and verify it.

When reviewing code, be direct about compile errors, index bugs, boundary
conditions, and invariants. Prefer small, local fixes over broad refactors.

## Git Notes

- Run git commands inside `/home/xiaodong/rrison/algorithms4th`, not the parent
  tree.
- Do not commit or push unless the user explicitly asks.
- Preserve unrelated local edits. Check `git status --short` before staging.
