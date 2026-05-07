# Contributing

Current audience: primarily solo/internal development, with PR-style discipline.

## Flow

1. Start from a named issue or explicit task.
2. Branch from `master`.
3. Make scoped changes.
4. Run validation:
   - `cargo test -p retrospective_core`
   - if SDK hooks changed: phase 2 build + launch smoke test
5. Update docs when behavior/workflow/ABI changes.
6. Open PR into `master` with validation evidence.

## Code Style

- Rust: `cargo fmt` + tests.
- C++: preserve local file style and keep edits narrowly scoped.
- Prefer explicit ownership and lifecycle boundaries at the C++/Rust seam.

## Documentation Rules

- `docs/NOW.md`: current direction and active scope.
- `docs/ARCHITECTURE.md`: layers and boundaries.
- `docs/WORKFLOWS.md`: command loops and Git flow.
- `docs/CONTRACTS.md`: ABI compatibility policy.
- `docs/DECISIONS.md`: durable policy decisions.
