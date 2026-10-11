#!/bin/bash
# Push every local store entry the remote store does not have yet.
#
#   ./dev store-publish [--registry <remote>]
#
# The remote is TAU_STORE_REMOTE, or --registry. This is the only writer of
# the remote store; only the trusted workflow on devel and main calls it. The
# caller builds the packages first (a configure with TAU_STORE_REMOTE set, or
# without it), so a run only uploads what the remote lacks.
set -u

source "$(dirname "${BASH_SOURCE[0]}")/env"

args=()
while [ "$#" -gt 0 ]; do
	case "$1" in
		--registry)
			[ "$#" -ge 2 ] || { echo "store-publish: --registry needs a value" >&2; exit 2; }
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

if [ -z "${TAU_STORE_REMOTE:-}" ]; then
	echo "store-publish: set TAU_STORE_REMOTE or pass --registry" >&2
	exit 2
fi

prefix="$(dep_shared_prefix)" || exit 1

pushed=0
while IFS= read -r entry; do
	[ -n "$entry" ] || continue
	if "${DEV_ROOT}/scripts/store-remote.sh" has "$prefix" "$entry"; then
		continue
	fi
	"${DEV_ROOT}/scripts/store-remote.sh" push "$prefix" "$entry" || exit 1
	pushed=$((pushed + 1))
done < <("${DEV_ROOT}/scripts/store-remote.sh" local-entries "$prefix")

echo "store-publish: pushed ${pushed} entries to ${TAU_STORE_REMOTE}" >&2
