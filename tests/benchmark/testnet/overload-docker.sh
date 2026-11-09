#!/bin/bash
# Run the tau-overload benchmarks of this folder on the tau of the checkout in
# the current directory, in Docker, and export their results:
#
#   <tooling>/tests/benchmark/testnet/overload-docker.sh <results dir> [docker build args...]
#
# The overload stage (overload.dockerfile) is appended to the Dockerfile of
# the checkout, so this script measures any commit whose Dockerfile has the
# build-compile stage, older commits included. It passes the build arguments
# of `./dev docker build-release-measured`, so after a measured suite run of
# the same checkout the compiled tree comes from the BuildKit cache. The exit
# status is the one of run_overload.py.
set -euo pipefail

here="$(cd "$(dirname "$0")" && pwd)"
results="${1:?usage: overload-docker.sh <results dir> [docker build args...]}"
shift

[ -f Dockerfile ] || { echo "overload-docker.sh: run it from the root of a tau-lang checkout" >&2; exit 2; }

composed="$(mktemp)"
trap 'rm -f "${composed}"' EXIT
cat Dockerfile "${here}/overload.dockerfile" > "${composed}"

# The version stamps of scripts/docker.sh: the build context carries no .git.
git_args=()
if git rev-parse --is-inside-work-tree > /dev/null 2>&1; then
	git_args+=(
		--build-arg "TAU_GIT_DESCRIBED=$(git describe --tags --always)"
		--build-arg "TAU_GIT_BRANCH=$(git rev-parse --abbrev-ref HEAD)"
		--build-arg "TAU_GIT_COMMIT_HASH=$(git log -1 --format=%h)"
	)
fi
if git -C external/parser rev-parse --is-inside-work-tree > /dev/null 2>&1; then
	git_args+=(
		--build-arg "TAU_PARSER_GIT_DESCRIBED=$(git -C external/parser describe --tags --always)"
		--build-arg "TAU_PARSER_GIT_BRANCH=$(git -C external/parser rev-parse --abbrev-ref HEAD)"
		--build-arg "TAU_PARSER_GIT_COMMIT_HASH=$(git -C external/parser log -1 --format=%h)"
		--build-arg "TAU_PARSER_COMMIT=$(git -C external/parser rev-parse HEAD)"
	)
fi

docker buildx build --progress="${PROGRESS:-plain}" ${git_args[@]+"${git_args[@]}"} \
	-f "${composed}" --target overload-results \
	--build-context "overload-tools=${here}" \
	--build-arg BUILD_PRESET=release --build-arg TAU_TEST_MEASURE=ON \
	--output "type=local,dest=${results}" "$@" .

status="$(cat "${results}/status")"
echo "run_overload.py exit status: ${status} (results in ${results})"
exit "${status}"
