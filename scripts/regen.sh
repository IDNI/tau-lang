#!/bin/bash

# Regenerates every grammar's parser: the core ones in parser/ and each BA's,
# which lives in its own plugin directory.

set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$root"

shopt -s nullglob
for tgf in parser/*.tgf src/boolean_algebras/*/parser/*.tgf; do
	./parser/gen "$tgf"
done
