#!/bin/bash

source "$(dirname "${BASH_SOURCE[0]}")/env"

dev_reject_build_type relwithdebinfo RelWithDebInfo "$@"
./dev build RelWithDebInfo "$@"
