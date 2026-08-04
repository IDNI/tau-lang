# CLAUDE.md

## Project Overview

Tau is an expressive, decidable, and executable formal software specification language. It supports satisfiability checking and program synthesis from logical specifications. The codebase is C++23, built with CMake, and targets Linux/Windows.

## Building and Testing

Always build using the `./dev` tool — run `./dev` with no arguments for the script list, and see `scripts/README.md` for CMake option handling.

Always run tests in debug AND release mode to ensure proper behavior in both cases.

## Code Conventions

- `.tmpl.h` files contain template implementations (included by their corresponding `.h`).
- Task annotations: `TODO`, `DOING`, `IDEA`, `FIXME`, `REVIEW`, `DOCUMENTATION`, `MARK` with priority tags `(IMPORTANT)`, `(HIGH)`, `(MEDIUM)`, `(LOW)`, `(VERY LOW)`.
- Don't hand-edit generated parser files (`*_parser.generated.h`) — regenerate them with `./dev regen`.
- External dependencies (CVC5, Boost) are installed to `~/.tau/` by `./dev dep-cvc5.sh` and `./dev dep-boost.sh`.
- The parser library is a git submodule at `external/parser/`.
