# C++ Learning Path (Rust-First)

This path is optimized for this repo's reality: thin C++ host code, Rust-heavy runtime logic.

## What To Learn First

1. C ABI boundary (`extern "C"`, symbol names, pointer ownership)
2. Dynamic loading (`dlopen`, `dlsym`, function pointers)
3. Lifetime basics (constructor/destructor, RAII)
4. Source SDK hook lifecycle (`DLLInit`, `LevelInit`, `GameFrame`, shutdown)

## Resource Stack

Primary references:

- LearnCpp (free modern tutorial): <https://www.learncpp.com/>
- C++ Core Guidelines: <https://isocpp.github.io/CppCoreGuidelines/>
- cppreference (language/library reference): <https://en.cppreference.com/w/cpp>
- ISO C++ FAQ: <https://isocpp.org/wiki/faq>

Books:

- A Tour of C++ (3rd): <https://www.stroustrup.com/tour3.html>
- Stroustrup books index: <https://www.stroustrup.com/books.html>

Toolchain support and diagnostics:

- Clang C++ status: <https://clang.llvm.org/cxx_status>
- GCC C++ status: <https://gcc.gnu.org/projects/cxx-status.html>
- C++20 compiler support matrix: <https://en.cppreference.com/w/cpp/compiler_support/20.html>
- clang-tidy docs: <https://clang.llvm.org/extra/clang-tidy/>
- AddressSanitizer docs: <https://clang.llvm.org/docs/AddressSanitizer.html>

## Repo-Mapped Study Order

1. Read `native/src/retrospective_loader.cpp` while reviewing C ABI basics.
2. Read `native/source_mod/sdk2013/retrospective_sdk_bridge.cpp` for engine-facing adapter patterns.
3. Read `external/source-sdk-2013/src/game/server/gameinterface.cpp` hook call sites (`retro_*` usage).
4. Read `rust/retrospective_core/src/lib.rs` to see boundary behavior from the Rust side.
5. Run `./scripts/run_phase1_harness.sh` after each conceptual step.

## Rule Of Thumb

Keep C++ as deterministic plumbing and keep game/runtime behavior in Rust whenever possible.
