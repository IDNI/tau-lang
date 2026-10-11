#!/bin/bash

set -euo pipefail

if [[ "${1:-}" = "all" ]]; then
	rm -rf ./build ./build-*
fi

rm -rf ./tau-config.cmake ./Testing
