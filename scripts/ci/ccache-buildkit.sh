#!/bin/bash
# ccache-buildkit.sh inject|extract <dir>
#
# Copies a ccache directory into, or out of, the BuildKit cache mount that the
# Dockerfile stages compile with (--mount=type=cache,target=/root/.ccache).
# The mount lives on the builder of the job, which the runner discards, so a
# job injects the cache before its docker build and extracts it after.
# Uses the builder docker buildx selects, the one setup-buildx-action made.
set -euo pipefail

mode="${1:?usage: ccache-buildkit.sh inject|extract <dir>}"
dir="${2:?usage: ccache-buildkit.sh inject|extract <dir>}"
image=ghcr.io/containerd/busybox:latest
# A new stamp each call, so BuildKit never reuses the copy step from its cache.
stamp="$(date +%s%N)"

case "$mode" in
	inject)
		mkdir -p "$dir"
		docker buildx build --progress=plain --build-arg "STAMP=${stamp}" -f - "$dir" <<EOF
FROM ${image}
ARG STAMP
RUN --mount=type=cache,target=/root/.ccache,sharing=locked \
	--mount=type=bind,source=.,target=/var/ccache-in \
	echo "inject \${STAMP}" && cp -p -R /var/ccache-in/. /root/.ccache/
EOF
		;;
	extract)
		empty="$(mktemp -d)"
		trap 'rm -rf "$empty"' EXIT
		rm -rf "$dir"
		mkdir -p "$dir"
		docker buildx build --progress=plain --build-arg "STAMP=${stamp}" \
			--output "type=local,dest=${dir}" -f - "$empty" <<EOF
FROM ${image} AS copy
ARG STAMP
RUN --mount=type=cache,target=/root/.ccache,sharing=locked \
	echo "extract \${STAMP}" && mkdir -p /out && cp -p -R /root/.ccache/. /out/
FROM scratch
COPY --from=copy /out/ /
EOF
		echo "ccache-buildkit: extracted $(du -sh "$dir" | cut -f1)"
		;;
	*)
		echo "ccache-buildkit: unknown mode ${mode}" >&2
		exit 2
		;;
esac
