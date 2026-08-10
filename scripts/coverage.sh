#!/bin/bash

set -euo pipefail

./dev build Coverage "$@"
cd build-Coverage
make coverage
