#!/bin/bash

source "$(dirname "${BASH_SOURCE[0]}")/env"

dev_pre_configure() {
	dep_entry "${DEV_CMAKE[@]}"
	echo "TAU_SHARED_PREFIX: $(dep_shared_prefix)"

	git_submodules_init
}

cd "${REPO_ROOT}"
build_entry "$@"
