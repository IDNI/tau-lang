#!/bin/bash
# Pull store entries from the remote store into the local store.
#
#   ./dev store-pull <dep>/<id>...
#
# With no entry the command does nothing: configure already reads from the
# remote on a local miss (cmake/tau-deps.cmake), so this only prefetches an
# explicitly named entry. TAU_STORE_REMOTE names the remote; --registry
# overrides it.
set -u

source "$(dirname "${BASH_SOURCE[0]}")/env"

args=()
while [ "$#" -gt 0 ]; do
	case "$1" in
		--registry)
			[ "$#" -ge 2 ] || { echo "store-pull: --registry needs a value" >&2; exit 2; }
			export TAU_STORE_REMOTE="$2"
			shift 2
			;;
		--registry=*)
			export TAU_STORE_REMOTE="${1#--registry=}"
			shift
			;;
		*)
			args+=("$1")
			shift
			;;
	esac
done

if [ "${#args[@]}" -gt 0 ]; then
	dep_entry "${args[@]}"
else
	dep_entry
fi

if [ "${#DEP_POSITIONAL[@]}" -eq 0 ]; then
	exit 0
fi
if [ -z "${TAU_STORE_REMOTE:-}" ]; then
	echo "store-pull: set TAU_STORE_REMOTE or pass --registry" >&2
	exit 2
fi

prefix="$(dep_shared_prefix)" || exit 1
for entry in "${DEP_POSITIONAL[@]}"; do
	"${DEV_ROOT}/scripts/store-remote.sh" pull "$prefix" "$entry" || exit 1
done
