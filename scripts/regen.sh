#!/bin/bash
# Regenerate every checked-in parser/*_parser.generated.h from its grammar.
# Keep this list in sync with the generate_parser() block in CMakeLists.txt.

set -e

for g in tau sbf bitvector adt qint qlt hsb nlang hoa; do
	echo "regenerating parser/${g}_parser.generated.{h,cpp}"
	./parser/gen "parser/${g}.tgf" --header-only false
done
