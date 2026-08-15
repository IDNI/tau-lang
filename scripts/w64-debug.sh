#!/bin/bash

source "$(dirname "${BASH_SOURCE[0]}")/env"

dev_reject_build_type w64-debug Debug "$@"
./dev w64-build Debug "$@"
