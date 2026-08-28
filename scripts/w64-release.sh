#!/bin/bash

source "$(dirname "${BASH_SOURCE[0]}")/env"

dev_reject_build_type w64-release Release "$@"
./dev w64-build Release "$@"
