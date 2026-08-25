#!/bin/bash
# Regenerate every checked-in parser/*_parser.generated.h from its grammar.
# Keep this list in sync with the generate_parser() block in CMakeLists.txt.

set -e

for g in tau sbf bitvector qint qlt hsb nlang; do
	echo "regenerating parser/${g}_parser.generated.h"
	./parser/gen "parser/${g}.tgf"
done
