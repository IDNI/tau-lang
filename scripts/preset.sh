#!/bin/bash

source "$(dirname "${BASH_SOURCE[0]}")/env"

PRESET_RUN_BIN="tau"
cd "${REPO_ROOT}"
preset_entry "$@"
