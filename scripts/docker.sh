#!/bin/bash

CMD="${1:-help}"
PROGRESS="auto" # "auto" or "plain"

build() {
        local git_args=()
        if git rev-parse --is-inside-work-tree > /dev/null 2>&1; then
                git_args=(
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
                )
        fi
        echo "Building: '${@}'"
        docker buildx build --progress=${PROGRESS} "${git_args[@]}" "$@" .
}

run() {
        echo "Running docker run --rm -it ${@}"
        docker run --rm -it "$@"
}

extract-packages() {
        echo "Extracting packages: ${@}"
        local IMAGE=$1
        CID=$(docker create $IMAGE)
        mkdir -p build-Release
        docker cp $CID:/root/.tau/packages ~/.tau
        docker rm $CID
}

case "${CMD}" in
        "help")
                echo "Usage: ./dev docker <ACTION> <DOCKER OPTIONS>"
                echo "Available actions runs docker build:"
                echo "  tau           - build tau runner image w/o tests and run it"
                echo "  tau-rpm       - build tau rpm runner image w/o tests and run it"
                echo "  run           - run a container"
                echo "  bash          - run bash in a container"
                echo "  build         - default build"
                echo "  base          - build base image"
                echo "  deps          - build deps image"
                echo "  build-debug   - build build image with debug tests"
                echo "  build-release - build build image with release tests"
                echo "  test-with-testnet - run the tau-testnet suite against the build"
                echo "  packages      - build packages image"
                echo "  nightly       - build nightly packages image"
                echo "  w64-deps      - build w64-deps image"
                echo "  w64-build     - build w64-build image"
                echo "  w64-packages  - release packages (Windows)"
                echo "  w64-nightly   - nightly release packages (Windows)"
                echo "  wasm-deps         - build wasm-deps image"
                echo "  wasm-node         - build wasm-node image, and (TESTS=yes) run the node suite"
                echo "  wasm-browser-deps - build wasm-browser-deps image (Chrome, npm packages)"
                echo "  wasm-browser      - build wasm-browser image, and (TESTS=yes) run the browser suite"
                ;;
        "build")
                build "${@:2}"
                ;;
        "run")
                run "${@:2}"
                ;;
        "bash")
                run --entrypoint "/bin/bash" "${@:2}"
                ;;
        # tau-runner (deb based) without building tests
        "tau")
                build --target runner --build-arg TESTS="no" -t tau "${@:2}" && \
                run -t tau
                ;;
        "tau-runner")
                build --target runner -t tau:runner "${@:2}" && \
                run -t tau:runner
                ;;
        "tau-rpm-runner")
                build --target rpm-runner -t tau:rpm-runner "${@:2}" && \
                run -t tau:rpm-runner
                ;;
        "base")
                build --target base -t tau:base "${@:2}"
                ;;
        "deps")
                build --target deps -t tau:deps "${@:2}"
                ;;
        "build-debug")
                build --target build --build-arg BUILD_PRESET="debug" -t tau:debug "${@:2}"
                ;;
        "build-release")
                build --target build --build-arg BUILD_PRESET="release" -t tau:build "${@:2}"
                ;;
        "test-with-testnet")
                build --target testnet -t tau:testnet "${@:2}"
                ;;
        "packages")
                build --target packages -t tau:packages "${@:2}" && \
                extract-packages tau:packages
                ;;
        "nightly")
                build --target packages --build-arg NIGHTLY="yes" -t tau:packages "${@:2}" && \
                extract-packages tau:packages
                ;;
        "w64-deps")
                build --target w64-deps -t tau:w64-deps "${@:2}"
                ;;
        "w64-build")
                build --target w64-build -t tau:w64-build "${@:2}"
                ;;
        "w64-packages")
                build --target w64-packages -t tau:w64-packages "${@:2}" && \
                extract-packages tau:w64-packages
                ;;
        "w64-nightly")
                build --target w64-packages --build-arg NIGHTLY="yes" -t tau:w64-packages "${@:2}" && \
                extract-packages tau:w64-packages
                ;;
        "wasm-deps")
                build --target wasm-deps -t tau:wasm-deps "${@:2}"
                ;;
        "wasm-node")
                build --target wasm-node -t tau:wasm-node "${@:2}"
                ;;
        "wasm-browser-deps")
                build --target wasm-browser-deps -t tau:wasm-browser-deps "${@:2}"
                ;;
        "wasm-browser")
                build --target wasm-browser -t tau:wasm-browser "${@:2}"
                ;;
        *)
                echo "Unknown docker action: ${CMD}"
                exit 1
                ;;
esac
