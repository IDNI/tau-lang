#!/bin/bash
# ccache-remote.sh pull|push <name> <dir>
#
# Keeps a ccache directory in the OCI registry of TAU_STORE_REMOTE, under the
# tag ccache-<name>, one tag per job, overwritten by each push. Every branch
# and pull request reads the same tag, which the actions cache cannot offer:
# its entries are visible only to their branch and the default branch, and it
# evicts above 10 GB. Needs oras on PATH and a login to the registry.
#
# A failure never fails the job: a missing or unreadable cache is a cold
# build, and a failed push loses only the speed-up of the next run.
set -uo pipefail

mode="${1:?usage: ccache-remote.sh pull|push <name> <dir>}"
name="${2:?usage: ccache-remote.sh pull|push <name> <dir>}"
dir="${3:?usage: ccache-remote.sh pull|push <name> <dir>}"
remote="${TAU_STORE_REMOTE:?ccache-remote: set TAU_STORE_REMOTE}"

# An OCI repository name is lower case; the registry half keeps its case.
case "$remote" in
	*/*) ref="${remote%%/*}/$(printf '%s' "${remote#*/}" | tr '[:upper:]' '[:lower:]')" ;;
	*)   ref="$(printf '%s' "$remote" | tr '[:upper:]' '[:lower:]')" ;;
esac
ref="${ref}:ccache-${name}"

# Git Bash on a Windows runner sees RUNNER_TEMP as a Windows path.
base="${RUNNER_TEMP:-${TMPDIR:-/tmp}}"
if command -v cygpath >/dev/null 2>&1; then base="$(cygpath -u "$base")"; fi
tmp="$(mktemp -d "${base}/ccache-remote.XXXXXX")" || exit 0
trap 'rm -rf "$tmp"' EXIT

case "$mode" in
	pull)
		mkdir -p "$dir"
		# oras names the pulled file after the pushed one, ccache.tar.
		if (cd "$tmp" && oras pull "$ref" -o .) >&2 && [ -f "$tmp/ccache.tar" ]; then
			tar -xf "$tmp/ccache.tar" -C "$dir" \
				&& echo "ccache-remote: restored ${ref} ($(du -sh "$dir" | cut -f1))" \
				|| echo "::warning::ccache-remote: cannot unpack ${ref}; the build starts cold"
		else
			echo "::notice::ccache-remote: no cache at ${ref}; the build starts cold"
		fi
		;;
	push)
		if [ ! -d "$dir" ] || [ -z "$(ls -A "$dir")" ]; then
			echo "ccache-remote: ${dir} is empty, nothing to push"
			exit 0
		fi
		# ccache compresses each entry already, so the tar stays plain.
		tar -cf "$tmp/ccache.tar" -C "$dir" . || { echo "::warning::ccache-remote: cannot pack ${dir}"; exit 0; }
		# oras rejects an absolute file path, so it pushes from the folder.
		if (cd "$tmp" && oras push --artifact-type "application/vnd.tau.ccache.v1" \
				"$ref" ccache.tar) >&2; then
			echo "ccache-remote: pushed ${ref} ($(du -h "$tmp/ccache.tar" | cut -f1))"
		else
			echo "::warning::ccache-remote: cannot push ${ref}"
		fi
		;;
	*)
		echo "ccache-remote: unknown mode ${mode}" >&2
		exit 2
		;;
esac
exit 0
