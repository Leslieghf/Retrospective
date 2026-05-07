# C++ vs Rust Mental Model

This is a practical map for this project, not a full language spec.

## Build and Tooling

- Rust: Cargo is build tool + package manager + test runner.
- C++: CMake generates build files; compiler/linker do the build.
- In this repo, C++ playground avoids third-party packages entirely.

## Ownership and Lifetimes

- Rust enforces ownership/borrowing rules at compile time.
- C++ has no borrow checker. You enforce lifetime discipline manually.
- Use RAII and prefer value types.
- Avoid raw `new`/`delete`; prefer stack objects and standard containers.

## Mutation and Aliasing

- Rust references have strict aliasing rules (`&T` vs `&mut T`).
- C++ references/pointers can alias freely, so bugs are easier to create.
- Use `const` aggressively and keep mutation narrow.

## Nullability

- Rust: `Option<T>`.
- C++: raw pointers may be null; references are expected non-null.
- Prefer `std::optional<T>` for value-level optionality.

## Error Handling

- Rust: `Result<T, E>` by default.
- C++: return codes, exceptions, or both depending on project policy.
- In this repo's bridge code, return/status codes are the current contract.

## Concurrency

- Rust blocks many race patterns at compile time.
- C++ allows data races that become undefined behavior.
- Treat shared mutable state as a high-risk area.

## "Safe Subset" Rules For This Repo

1. Prefer standard library over custom low-level memory code.
2. Prefer `std::vector`, `std::string`, `std::array`, `std::optional`, `std::span`.
3. Avoid manual memory ownership unless absolutely necessary.
4. Keep C++ bridge code thin; keep runtime logic in Rust.
