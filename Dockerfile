# This is a Dockerfile for building, testing, packaging and running the tau-lang

# To build the image with tag 'tau' from ./Dockerfile:
#   docker build --target runner -t tau .

# This Dockerfile contains the following stages (--target <stage>):
# - base: contains the base system with installed system dependencies
# - Linux branch is:
#   - deps: dependencies (cvc5)
#   - build: builds tau executable and runs tests
#   - packages: creates a release packages (deb and rpm)
#   - testnet: runs the tau-testnet suite against the build
#   - asan: builds and runs the tests with the address sanitizer
#   - runner: provides a deb based image with installed tau package
#   - rpm-runner: provides an rpm based image with installed tau package
# - Windows branch is:
#   - w64-deps: dependencies built with mingw-w64 (cvc5 and boost)
#   - w64-build: builds tau executable for Windows, and (TESTS=yes) runs its suite under wine
#   - w64-packages: creates a release packages (installer and zip file)
# - WebAssembly branch is (see AGENTS.md's WebAssembly section for the constraints):
#   - wasm-deps: extends the native dependencies with emsdk
#   - wasm-node: builds tau.js/tau.wasm/tau.esm.mjs and (TESTS=yes) runs the node
#     suite, the no-thread configuration and the wasm-vs-native parity check
#   - wasm-browser-deps: adds Chrome for Testing, puppeteer-core and xterm.js
#   - wasm-browser: builds the browser REPL page and (TESTS=yes) runs the
#     compiled suite in headless Chrome, the REPL suite inside the page, and
#     the REPL page start check
# - build, w64-build and wasm-node each split in two:
#   - <stage>-resolve: configures only, which resolves the store packages. With
#     TAU_STORE_PUBLISH=ON, configure publishes each entry right after it builds it
#   - <stage>: compiles and runs the tests on top of <stage>-resolve
# - build is split once more: build-compile compiles on top of build-resolve and
#   build runs the tests on top of build-compile, so the testnet stage can reuse
#   the compiled tree without running the tau suite

# use --build-arg BUILD_JOBS=N to set the number of build jobs (default is 5, 0 is for half of the available logical CPU cores)
# use --build-arg BUILD_PRESET="debug" for building of the debugging version (build stage)
# use --build-arg TESTS="no" to skip running tests (build, w64-build, wasm-node, wasm-browser)
# use --build-arg TEST_GCC_BUILD="no" to skip checking compilation with gcc (build stage)
# use --build-arg NIGHTLY="yes" to build a nightly package (packages and w64-packages stages)

# Use BUILD_KIT=1 (install docker-buildx) to avoid rebuilds of unnecessary stages

# Anonymous clones of the cvc5/boost repos can be rate limited by GitHub.
# Pass an optional token to authenticate:
#   --secret id=gh_token,env=GH_TOKEN
# Without the secret the clones run anonymously.

# To run tau using the created image in interactive mode:
#   docker run --rm -it tau [<tau options>]

# --rm flag is used to remove the container after it exits

# Number of build jobs (0 to use all available logical CPU cores)
ARG BUILD_JOBS=5

# ============================================================

# ------------------------------------------------------------
# base image with system dependencies
# pinned by digest: an upstream retag of the floating tag invalidates
# this layer and every one after it, wiping the cvc5/boost deps cache
FROM ubuntu:24.04@sha256:224a1869083a311ef3f13648a154ba79832fbef6364d31493642ca03082da254 AS base

# Install dependencies
# python3-dev and python3-venv stay for the tau-testnet stage, whose venv is
# made from the system interpreter. nanobind never reaches the system Python:
# the deps stage makes the shared venv the binding builds against.
RUN echo "(BUILD) -- Installing dependencies" && \
	apt-get update && apt-get install -y \
	bash wget git gnupg nsis rpm ninja-build bison ccache curl ca-certificates unzip \
	python3-pip python3-venv python3-dev \
	cmake=3.28.3-1build7 \
	g++=4:13.2.0-7ubuntu1 \
	mingw-w64=11.0.1-3build1 \
	clang-19=1:19.1.1-1ubuntu1~24.04.2

# The presets and the host tgf probe need the plain names, and every stage inherits them.
RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-19 100 && \
	update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-19 100

# create tau-lang directory and set it as the working directory
RUN echo "(BUILD) -- Creating /tau-lang and home directory" && \
	mkdir -p /tau-lang ~/.tau


# ------------------------------------------------------------
# Source stage to copy from (in order to COPY from building host only once)

FROM base AS source

# Copy the whole tree, so a new top-level directory cannot be left out.
# .dockerignore keeps .git, the build trees and .local out.
COPY ./ /tau-lang

RUN cd /tau-lang && ./dev clean all


# ============================================================

# ------------------------------------------------------------
# Linux dependencies image

FROM base AS deps

ARG BUILD_JOBS=5

# Dependencies resolve through the store at configure time
# (cmake/tau-deps.cmake), never a pre-source dist layer. This stage installs
# oras, the pinned client configure uses to read a missing package from the
# remote store, and the shared Python 3.12 venv the binding builds against.
# dep-oras.sh verifies its release by sha256, dep-python-venv.sh pins its uv
# and its packages.
COPY ./dev /tau-lang/
COPY ./external/parser/scripts/devrc \
	./external/parser/scripts/dep-build \
	/tau-lang/external/parser/scripts/
COPY ./external/parser/cmake/tau-resolve.cmake /tau-lang/external/parser/cmake/
COPY ./scripts/env /tau-lang/scripts/
COPY ./scripts/dep-oras.sh /tau-lang/scripts/
COPY ./scripts/dep-python-venv.sh /tau-lang/scripts/
RUN cd /tau-lang && ./dev dep-oras
RUN cd /tau-lang && ./dev dep-python-venv

# Every stage that builds the binding takes the interpreter from here. The
# shared prefix is /root/.tau in this image, so this is the venv the line
# above created.
ENV TAU_PYTHON=/root/.tau/py312/bin/python3


# ------------------------------------------------------------
# Build tau executable and its tests (if TESTS = "yes"). The store packages
# resolve and publish here, before any test runs.

FROM deps AS build-resolve

COPY --from=source /tau-lang /tau-lang

WORKDIR /tau-lang

# The build context carries no .git, so the stamp arrives as a build argument.
ARG TAU_GIT_DESCRIBED=
ARG TAU_GIT_BRANCH=
ARG TAU_GIT_COMMIT_HASH=
ARG TAU_PARSER_GIT_DESCRIBED=
ARG TAU_PARSER_GIT_BRANCH=
ARG TAU_PARSER_GIT_COMMIT_HASH=
ARG TAU_PARSER_COMMIT=
ENV TAU_GIT_DESCRIBED=${TAU_GIT_DESCRIBED} \
	TAU_GIT_BRANCH=${TAU_GIT_BRANCH} \
	TAU_GIT_COMMIT_HASH=${TAU_GIT_COMMIT_HASH} \
	TAU_PARSER_GIT_DESCRIBED=${TAU_PARSER_GIT_DESCRIBED} \
	TAU_PARSER_GIT_BRANCH=${TAU_PARSER_GIT_BRANCH} \
	TAU_PARSER_GIT_COMMIT_HASH=${TAU_PARSER_GIT_COMMIT_HASH} \
	TAU_PARSER_COMMIT=${TAU_PARSER_COMMIT}

# The HTTP oracle in nlang links libcurl.
RUN apt-get update && apt-get install -y --no-install-recommends libcurl4-openssl-dev

# Argument NIGHTLY=yes is used to build nightly packages (works only if RELEASE=yes)
ARG NIGHTLY=no

# if NIGHTLY is set to yes, then add .YYYY-MM-DD to the first line of the VERSION file
RUN if [ "$NIGHTLY" = "yes" ]; then \
	echo -n "$(head -n 1 VERSION)-$(date --iso)" > VERSION; \
fi

ARG BUILD_JOBS=5

# Argument BUILD_PRESET=release/debug picks the CMake preset family
ARG BUILD_PRESET=release

# Argument TESTS=no is used to skip building and running tests
ARG TESTS=yes

# The remote store: configure reads a missing package from it before it builds
# one (cmake/tau-deps.cmake). The token is mounted only for the configure
# step; an empty value keeps the remote out of a local build.
ARG TAU_STORE_REMOTE=
ENV TAU_STORE_REMOTE=${TAU_STORE_REMOTE}

# Argument TAU_BAS=<ids> picks the pack; empty keeps the default pack
ARG TAU_BAS=

# The source COPY above changes on every commit, so no layer cache can hold
# the objects. A ccache mount survives it, and it outlives the build.
ENV CCACHE_DIR=/root/.ccache CCACHE_MAXSIZE=3G

# Set TEST_GCC_BUILD=no to skip the gcc compilation check
ARG TEST_GCC_BUILD=yes

# Resolve every store package this build reads, and nothing else: the compile
# lives in the build stage, which inherits this layer, so a compile failure
# never rebuilds a dependency. The gcc check compiles a second, gcc-built set
# of packages, so its configure belongs here to resolve and publish them. The
# *-all preset enables the executable and the tests in one configure.
# ON makes configure publish each store entry right after it builds it, so a
# later failure cannot lose it. The workflow turns it on when the guard allows.
ARG TAU_STORE_PUBLISH=OFF
RUN --mount=type=secret,id=gh_token \
	echo "(BUILD) -- Resolving ${BUILD_PRESET} dependencies: $(head -n 1 VERSION)" && \
	echo " (BUILD) -- Tests: $TESTS" && \
	if [ "$TESTS" = "yes" ]; then \
		scripts/with-gh-token ./dev preset ${BUILD_PRESET}-all --configure-only \
			-DTAU_BUILD_JOBS=${BUILD_JOBS} \
			${TAU_BAS:+-DTAU_BAS=${TAU_BAS}} \
			-DCMAKE_C_COMPILER_LAUNCHER=ccache \
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache; \
	else \
		scripts/with-gh-token ./dev preset ${BUILD_PRESET}-tau --configure-only \
			-DTAU_BUILD_JOBS=${BUILD_JOBS} \
			-DCMAKE_C_COMPILER_LAUNCHER=ccache \
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache; \
	fi && \
	if [ "$TESTS" = "yes" -a "$TEST_GCC_BUILD" = "yes" ]; then \
		scripts/with-gh-token ./dev preset devel-make-gcc --configure-only \
			-DTAU_BUILD_JOBS=${BUILD_JOBS} \
			-DCMAKE_C_COMPILER_LAUNCHER=ccache \
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache; \
	fi

# Compile the tree configured above. This is a stage of its own so testnet can
# reuse the compiled tree without running the tau suite.
FROM build-resolve AS build-compile

ARG BUILD_JOBS=5
ARG BUILD_PRESET=release
ARG TESTS=yes
ARG TAU_BAS=

# *-all enables the executable and the tests in one configure
RUN --mount=type=cache,target=/root/.ccache,sharing=locked \
	--mount=type=secret,id=gh_token \
	echo "(BUILD) -- Building ${BUILD_PRESET} version: $(head -n 1 VERSION)" && \
	echo " (BUILD) -- Building tests: $TESTS" && \
	if [ "$TESTS" = "yes" ]; then \
		scripts/with-gh-token ./dev preset ${BUILD_PRESET}-all -DTAU_BUILD_JOBS=${BUILD_JOBS} \
			${TAU_BAS:+-DTAU_BAS=${TAU_BAS}} \
			-DCMAKE_C_COMPILER_LAUNCHER=ccache \
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache; \
	else \
		scripts/with-gh-token ./dev preset ${BUILD_PRESET}-tau -DTAU_BUILD_JOBS=${BUILD_JOBS} \
			-DCMAKE_C_COMPILER_LAUNCHER=ccache \
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache; \
	fi && \
	ccache --show-stats

# Set TEST_GCC_BUILD=no to skip the gcc compilation check
ARG TEST_GCC_BUILD=yes

# Check also make and gcc compilation since ninja and clang is used by default
RUN --mount=type=cache,target=/root/.ccache,sharing=locked \
	--mount=type=secret,id=gh_token \
	if [ "$TESTS" = "yes" -a "$TEST_GCC_BUILD" = "yes" ]; then \
	scripts/with-gh-token ./dev preset devel-make-gcc -DTAU_BUILD_JOBS=${BUILD_JOBS} \
		-DCMAKE_C_COMPILER_LAUNCHER=ccache \
		-DCMAKE_CXX_COMPILER_LAUNCHER=ccache && \
	rm -rf build/devel-gcc; \
fi

# Run the tests (if TESTS = "yes") on the compiled tree above.
FROM build-compile AS build

ARG BUILD_JOBS=5
ARG BUILD_PRESET=release
ARG TESTS=yes

RUN if [ "$TESTS" = "yes" ]; then \
	echo "(BUILD) -- Running tests: ${BUILD_PRESET}-all" && \
	ctest --preset ${BUILD_PRESET}-all -j ${BUILD_JOBS} --output-on-failure; \
fi

# Set the entrypoint to the tau executable
WORKDIR /tau-lang/build/${BUILD_PRESET}
ENTRYPOINT [ "./tau" ]
CMD []

# ------------------------------------------------------------
# create Linux packages

FROM build AS packages

ARG BUILD_JOBS=5

# Argument TESTS=no is used to skip running tests
ARG TESTS=yes

WORKDIR /tau-lang

RUN echo "(BUILD) -- Building packages" && \
	./dev preset release-packages-deb -DTAU_BUILD_JOBS=${BUILD_JOBS} && \
	./dev preset release-packages-rpm -DTAU_BUILD_JOBS=${BUILD_JOBS}


# ------------------------------------------------------------
# Run the tau-testnet suite against the release build

# tau-testnet consumes the built tree and the binding, not the tau suite, so it
# inherits the compile stage and does not run ctest on the way in.
FROM build-compile AS testnet

ARG BUILD_JOBS=5

# Argument BUILD_PRESET=release/debug picks the CMake preset family
ARG BUILD_PRESET=release

WORKDIR /tau-lang

# This stage builds the binding against its own tau-testnet venv, which holds
# the interpreter and nanobind that suite runs; the image's venv is not it.
ENV TAU_PYTHON=

# The tau-testnet requirements build fastecdsa from source, which needs gmp
RUN apt-get update && apt-get install -y --no-install-recommends libgmp-dev

RUN --mount=type=cache,target=/root/.ccache,sharing=locked \
	--mount=type=secret,id=gh_token \
	scripts/with-gh-token ./dev test-with-tau-testnet \
		${BUILD_PRESET}-binding-python \
		-DTAU_BUILD_JOBS=${BUILD_JOBS} \
		-DCMAKE_C_COMPILER_LAUNCHER=ccache \
		-DCMAKE_CXX_COMPILER_LAUNCHER=ccache

# ------------------------------------------------------------
# Run the tests with the address sanitizer. The sanitizer flag is added after
# configure resolves the store, so this stage reuses the Linux store packages.

FROM deps AS asan

COPY --from=source /tau-lang /tau-lang

WORKDIR /tau-lang

# The build context carries no .git, so the stamp arrives as a build argument.
ARG TAU_GIT_DESCRIBED=
ARG TAU_GIT_BRANCH=
ARG TAU_GIT_COMMIT_HASH=
ARG TAU_PARSER_GIT_DESCRIBED=
ARG TAU_PARSER_GIT_BRANCH=
ARG TAU_PARSER_GIT_COMMIT_HASH=
ARG TAU_PARSER_COMMIT=
ENV TAU_GIT_DESCRIBED=${TAU_GIT_DESCRIBED} \
	TAU_GIT_BRANCH=${TAU_GIT_BRANCH} \
	TAU_GIT_COMMIT_HASH=${TAU_GIT_COMMIT_HASH} \
	TAU_PARSER_GIT_DESCRIBED=${TAU_PARSER_GIT_DESCRIBED} \
	TAU_PARSER_GIT_BRANCH=${TAU_PARSER_GIT_BRANCH} \
	TAU_PARSER_GIT_COMMIT_HASH=${TAU_PARSER_GIT_COMMIT_HASH} \
	TAU_PARSER_COMMIT=${TAU_PARSER_COMMIT}

# The HTTP oracle in nlang links libcurl.
RUN apt-get update && apt-get install -y --no-install-recommends libcurl4-openssl-dev

ARG BUILD_JOBS=5

ARG TAU_STORE_REMOTE=
ENV TAU_STORE_REMOTE=${TAU_STORE_REMOTE}

ENV CCACHE_DIR=/root/.ccache CCACHE_MAXSIZE=3G

# The launchers match build-resolve, so the store ids match the Linux job's.
# release-asan sets TAU_LTO=OFF, which moves only the parser SDK id.
ARG TAU_STORE_PUBLISH=OFF
RUN --mount=type=cache,target=/root/.ccache,sharing=locked \
	--mount=type=secret,id=gh_token \
	echo "(BUILD) -- Building and running the address sanitizer tests" && \
	scripts/with-gh-token ./dev preset release-asan-tests run \
		-DTAU_BUILD_JOBS=${BUILD_JOBS} \
		-DCMAKE_C_COMPILER_LAUNCHER=ccache \
		-DCMAKE_CXX_COMPILER_LAUNCHER=ccache && \
	ccache --show-stats


# ============================================================

# ------------------------------------------------------------
# Runner image

FROM debian:trixie-slim AS runner

# Copy the tau deb package
COPY --from=packages /root/.tau/packages/*.deb /

# Install it and remove the package
RUN dpkg -i /*.deb && rm -f /*.deb

# Set the entrypoint to the tau executable
WORKDIR /
ENTRYPOINT [ "/usr/bin/tau" ]
CMD []

# ============================================================

# ------------------------------------------------------------
# RPM runner image

FROM fedora:40 AS rpm-runner

# Copy the tau deb package
COPY --from=packages /root/.tau/packages/*.rpm /

# Install it and remove the package
RUN rpm -i /*.rpm && rm -f /*.rpm

# Set the entrypoint to the tau executable
WORKDIR /
ENTRYPOINT [ "/usr/bin/tau" ]
CMD []


# ============================================================

# ------------------------------------------------------------
# Windows dependencies image

FROM deps AS w64-deps

# The w64 packages have their own content ids (target=windows-x86_64-mingw) in
# the same store as the native ones, and oras is already installed in the parent
# stage. This stage exists so the w64 image stays a separate, cacheable step
# from the Linux one; configure builds a w64 package the remote does not have.
ARG BUILD_JOBS=5

# wine runs the w64 suite. wine, not wine64: Ubuntu's wine package runs 64-bit
# PE executables on its own, with no i386 multiarch for mingw-w64-x86_64 binaries.
RUN apt-get update && apt-get install -y --no-install-recommends wine

# WINEPREFIX keeps the wine configuration out of the home directory.
# WINEDEBUG drops wine's own noise, and not the output of a test.
ENV WINEPREFIX=/root/.wine-tau WINEDEBUG=-all

# Parallel tests that each find no prefix all create it at once, and their
# wine servers fail to start. Create the prefix once, here.
RUN wineboot -i && wineserver -w

# Ubuntu 24.04's mingw-w64 GCC 13 defines std::type_info::operator== twice in a
# C++23 static link (GCC PR 110572). Take GCC 16 from Debian forky by sha256, from
# snapshot.debian.org, which keeps every version. Drop Ubuntu's trees first.
RUN set -eux; \
	test "$(dpkg --print-architecture)" = amd64; \
	rm -rf /usr/x86_64-w64-mingw32 /usr/lib/gcc/x86_64-w64-mingw32; \
	rm -rf /usr/share/doc/*mingw-w64*; \
	mkdir -p /tmp/mingw-debs; cd /tmp/mingw-debs; \
	printf '%s\n' \
		'46d240cdc2d3ce92503e0ff647ac7f7edc385d8affb833903ade15c882a6b6b8  https://snapshot.debian.org/archive/debian/20260920T143653Z/pool/main/b/binutils-mingw-w64/binutils-mingw-w64-base_2.47-6+13.3+b1_amd64.deb' \
		'7ee8b4c8cce93221606d6eb1ceed01c821eef4f178cceb6f7bccde5757fd6399  https://snapshot.debian.org/archive/debian/20260920T143653Z/pool/main/b/binutils-mingw-w64/binutils-mingw-w64-x86-64_2.47-6+13.3+b1_amd64.deb' \
		'6ef2784e073e7de3a4946918b51d4e66a49c5e964e6e7e231aad4fc662059a01  https://snapshot.debian.org/archive/debian/20260920T143653Z/pool/main/g/gcc-mingw-w64/g++-mingw-w64-x86-64-win32_16.2.0-1+29_amd64.deb' \
		'f09078ea8108135c0b2d501b735d8c1f15f752f1f414dd51a7ebfffb303fca51  https://snapshot.debian.org/archive/debian/20260920T143653Z/pool/main/g/gcc-mingw-w64/gcc-mingw-w64-base_16.2.0-1+29_amd64.deb' \
		'14f6601dad10b5b73b1a7db5aaee18a1ef56f5cf9010b312ff30633322c4890c  https://snapshot.debian.org/archive/debian/20260920T143653Z/pool/main/g/gcc-mingw-w64/gcc-mingw-w64-x86-64-win32-runtime_16.2.0-1+29_amd64.deb' \
		'99ae948f575b53d6eca7728988605372d536284dbe8d6ed14bfe98074afeda1a  https://snapshot.debian.org/archive/debian/20260920T143653Z/pool/main/g/gcc-mingw-w64/gcc-mingw-w64-x86-64-win32_16.2.0-1+29_amd64.deb' \
		'65a8c1261144617234f9b490d354e0a04f21649fe10c451a02fb0dc4d5db7b4d  https://snapshot.debian.org/archive/debian/20260920T143653Z/pool/main/m/mingw-w64/mingw-w64-common_14.0.0-1_all.deb' \
		'2802e3b2cc08c4732c1aae490457df5e8f2468ccb66d7974c8b4a76ea59ae449  https://snapshot.debian.org/archive/debian/20260920T143653Z/pool/main/m/mingw-w64/mingw-w64-x86-64-dev_14.0.0-1_all.deb' \
		> packages; \
	while read -r sha url; do \
		file="$(basename "$url")"; \
		curl -fsSL -o "$file" "$url"; \
		echo "$sha  $file" | sha256sum -c --quiet -; \
	done < packages; \
	for deb in *.deb; do dpkg -x "$deb" /; done; \
	cd /; rm -rf /tmp/mingw-debs; \
	x86_64-w64-mingw32-g++ --version; \
	x86_64-w64-mingw32-ld --version


# ------------------------------------------------------------
# Windows build image: builds tau and its tests (if TESTS = "yes"), so the
# store packages resolve before any test runs

FROM w64-deps AS w64-build-resolve

COPY --from=source /tau-lang /tau-lang

WORKDIR /tau-lang

# The build context carries no .git, so the stamp arrives as a build argument.
ARG TAU_GIT_DESCRIBED=
ARG TAU_GIT_BRANCH=
ARG TAU_GIT_COMMIT_HASH=
ARG TAU_PARSER_GIT_DESCRIBED=
ARG TAU_PARSER_GIT_BRANCH=
ARG TAU_PARSER_GIT_COMMIT_HASH=
ARG TAU_PARSER_COMMIT=
ENV TAU_GIT_DESCRIBED=${TAU_GIT_DESCRIBED} \
	TAU_GIT_BRANCH=${TAU_GIT_BRANCH} \
	TAU_GIT_COMMIT_HASH=${TAU_GIT_COMMIT_HASH} \
	TAU_PARSER_GIT_DESCRIBED=${TAU_PARSER_GIT_DESCRIBED} \
	TAU_PARSER_GIT_BRANCH=${TAU_PARSER_GIT_BRANCH} \
	TAU_PARSER_GIT_COMMIT_HASH=${TAU_PARSER_GIT_COMMIT_HASH} \
	TAU_PARSER_COMMIT=${TAU_PARSER_COMMIT}

# Argument NIGHTLY=yes is used to build nightly packages (works only if RELEASE=yes)
ARG NIGHTLY=no

# if NIGHTLY is set to yes, then add .YYYY-MM-DD to the first line of the VERSION file
RUN if [ "$NIGHTLY" = "yes" ]; then \
	echo -n "$(head -n 1 VERSION)-$(date --iso)" > VERSION; \
fi

ARG BUILD_JOBS=5

# Argument BUILD_PRESET=release/debug picks the CMake preset family
ARG BUILD_PRESET=release

# Argument TESTS=no is used to skip building and running tests
ARG TESTS=yes

# The remote store: configure reads a missing w64 package from it before it
# builds one (cmake/tau-deps.cmake).
ARG TAU_STORE_REMOTE=
ENV TAU_STORE_REMOTE=${TAU_STORE_REMOTE}

# A source change misses every object layer, so the compiled objects live in a
# cache mount the CI carries across runs, as on the native build.
ENV CCACHE_DIR=/root/.ccache CCACHE_MAXSIZE=3G

# Resolve the w64 store packages, and nothing else: the compile lives in the
# w64-build stage, which inherits this layer, so a compile failure never
# rebuilds a dependency.
# ON makes configure publish each store entry right after it builds it, so a
# later failure cannot lose it. The workflow turns it on when the guard allows.
ARG TAU_STORE_PUBLISH=OFF
RUN --mount=type=secret,id=gh_token \
	echo "(BUILD) -- Resolving w64 ${BUILD_PRESET} dependencies: $(head -n 1 VERSION)" && \
	echo " (BUILD) -- Tests: $TESTS" && \
	if [ "$TESTS" = "yes" ]; then \
		scripts/with-gh-token ./dev preset ${BUILD_PRESET}-w64 --configure-only \
			-DTAU_BUILD_JOBS=${BUILD_JOBS} \
			-DTAU_BUILD_EXECUTABLE=ON -DTAU_BUILD_TESTS=ON \
			-DCMAKE_CROSSCOMPILING_EMULATOR=wine \
			-DCMAKE_C_COMPILER_LAUNCHER=ccache \
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache; \
	else \
		scripts/with-gh-token ./dev preset ${BUILD_PRESET}-w64 --configure-only \
			-DTAU_BUILD_JOBS=${BUILD_JOBS} \
			-DTAU_BUILD_EXECUTABLE=ON \
			-DCMAKE_C_COMPILER_LAUNCHER=ccache \
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache; \
	fi

# Compile the tree configured above, then run the w64 suite under wine
# (if TESTS = "yes").
FROM w64-build-resolve AS w64-build

ARG BUILD_JOBS=5
ARG BUILD_PRESET=release
ARG TESTS=yes

# Build tau executable, and its suite for wine if TESTS = "yes"
RUN --mount=type=cache,target=/root/.ccache,sharing=locked \
	--mount=type=secret,id=gh_token \
	echo "(BUILD) -- Building w64 ${BUILD_PRESET} version: $(head -n 1 VERSION)" && \
	echo " (BUILD) -- Building tests: $TESTS" && \
	scripts/with-gh-token ./dev preset ${BUILD_PRESET}-w64 -DTAU_BUILD_JOBS=${BUILD_JOBS} \
		-DTAU_BUILD_EXECUTABLE=ON \
		-DCMAKE_C_COMPILER_LAUNCHER=ccache \
		-DCMAKE_CXX_COMPILER_LAUNCHER=ccache && \
	if [ "$TESTS" = "yes" ]; then \
		scripts/with-gh-token ./dev preset ${BUILD_PRESET}-w64 -DTAU_BUILD_JOBS=${BUILD_JOBS} \
			-DTAU_BUILD_TESTS=ON -DCMAKE_CROSSCOMPILING_EMULATOR=wine \
			-DCMAKE_C_COMPILER_LAUNCHER=ccache \
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache; \
	fi && \
	ccache --show-stats

RUN if [ "$TESTS" = "yes" ]; then \
	echo "(BUILD) -- Running w64 tests under wine" && \
	ctest --test-dir build/${BUILD_PRESET}-w64 -j ${BUILD_JOBS} \
		--output-on-failure; \
fi


# ------------------------------------------------------------
# create Windows packages

FROM w64-build AS w64-packages

ARG BUILD_JOBS=5

WORKDIR /tau-lang

RUN echo "(BUILD) -- Building Windows packages" && \
	./dev preset release-w64-packages-zip -DTAU_BUILD_JOBS=${BUILD_JOBS} && \
	./dev preset release-w64-packages -DTAU_BUILD_JOBS=${BUILD_JOBS}


# ============================================================

# ------------------------------------------------------------
# WebAssembly dependencies image (emsdk and boost, no cvc5: the wasm pack
# is sbf,tau,qint,qlt)

FROM deps AS wasm-deps

ARG BUILD_JOBS=5

# ./dev, devrc and tau-resolve.cmake are already in place from the deps stage;
# only emsdk is new here. Boost, FTXUI, unordered_dense and the parser SDK come
# from the local store at configure time (cmake/tau-deps.cmake), never the
# older ~/.tau dist path.
COPY ./external/parser/scripts/dep-emsdk.sh 	/tau-lang/external/parser/scripts/
COPY ./scripts/dep-emsdk.sh	/tau-lang/scripts/
RUN echo "(BUILD) -- Building wasm dependencies: emsdk" && \
	cd /tau-lang && \
	./dev dep-emsdk.sh -DTAU_BUILD_JOBS=${BUILD_JOBS}

# emsdk ships the only node in the image, under a version directory whose name
# is not fixed. tests/CMakeLists.txt requires node on the path under Emscripten.
# dep-chrome.sh calls npx and dep-js-test-deps.sh calls npm.
RUN node_bin="$(ls -d /root/.tau/emsdk/node/*/bin | head -n1)" && \
	for tool in node npm npx; do \
		ln -s "${node_bin}/${tool}" "/usr/local/bin/${tool}"; \
	done


# ------------------------------------------------------------
# WebAssembly Node.js gate: build tau.js/tau.wasm/tau.esm.mjs and (with
# TESTS=yes) run the wasm suite under emsdk's node, the no-thread
# configuration, and the wasm-vs-native parity check. No Chrome or puppeteer.
# The -resolve stage builds everything, so the store packages resolve before
# any test runs.

FROM wasm-deps AS wasm-node-resolve

COPY --from=source /tau-lang /tau-lang

WORKDIR /tau-lang

# The build context carries no .git, so the stamp arrives as a build argument.
ARG TAU_GIT_DESCRIBED=
ARG TAU_GIT_BRANCH=
ARG TAU_GIT_COMMIT_HASH=
ARG TAU_PARSER_GIT_DESCRIBED=
ARG TAU_PARSER_GIT_BRANCH=
ARG TAU_PARSER_GIT_COMMIT_HASH=
ARG TAU_PARSER_COMMIT=
ENV TAU_GIT_DESCRIBED=${TAU_GIT_DESCRIBED} \
	TAU_GIT_BRANCH=${TAU_GIT_BRANCH} \
	TAU_GIT_COMMIT_HASH=${TAU_GIT_COMMIT_HASH} \
	TAU_PARSER_GIT_DESCRIBED=${TAU_PARSER_GIT_DESCRIBED} \
	TAU_PARSER_GIT_BRANCH=${TAU_PARSER_GIT_BRANCH} \
	TAU_PARSER_GIT_COMMIT_HASH=${TAU_PARSER_GIT_COMMIT_HASH} \
	TAU_PARSER_COMMIT=${TAU_PARSER_COMMIT}

ARG BUILD_JOBS=5

# Argument BUILD_PRESET=debug-wasm picks the debugging preset family
ARG BUILD_PRESET=release-wasm

# Argument TESTS=no builds only the library, skipping the suite entirely
ARG TESTS=yes

# The remote store: configure reads a missing wasm package from it before it
# builds one (cmake/tau-deps.cmake).
ARG TAU_STORE_REMOTE=
ENV TAU_STORE_REMOTE=${TAU_STORE_REMOTE}

# Native tau is the js_parity reference: tests/CMakeLists.txt registers that
# test only when TAU_PARITY_NATIVE_BIN exists, so it must be built before the
# wasm configure below. Its own store packages resolve in this layer.
# ON makes configure publish each store entry right after it builds it, so a
# later failure cannot lose it. The workflow turns it on when the guard allows.
ARG TAU_STORE_PUBLISH=OFF
RUN --mount=type=secret,id=gh_token \
	if [ "$TESTS" = "yes" ]; then \
	echo "(BUILD) -- Building native tau (sbf,tau pack) for parity" && \
	scripts/with-gh-token ./dev preset release-tau -DTAU_BAS=sbf,tau -DTAU_BUILD_JOBS=${BUILD_JOBS}; \
	fi

# Resolve the wasm store packages, and nothing else: the wasm compile lives in
# the wasm-node stage, which inherits this layer, so a compile failure never
# rebuilds an emscripten dependency. The -all-tests preset builds the library,
# the suite and the CLI from one configure.
#
# Tau's no-thread coverage is a droppable, no-SharedArrayBuffer configuration
# kept measured so it does not rot, because -pthread is the default for every
# other wasm target; its packages differ from the pthread ones.
RUN --mount=type=secret,id=gh_token \
	echo "(BUILD) -- Resolving the wasm dependencies" && \
	if [ "$TESTS" = "yes" ]; then \
		scripts/with-gh-token ./dev preset ${BUILD_PRESET}-all-tests --configure-only \
			-DTAU_BUILD_JOBS=${BUILD_JOBS} -DTAU_PARITY_REQUIRE_NATIVE=ON; \
	else \
		scripts/with-gh-token ./dev preset ${BUILD_PRESET} --configure-only \
			-DTAU_BUILD_JOBS=${BUILD_JOBS}; \
	fi && \
	if [ "$TESTS" = "yes" ]; then \
		scripts/with-gh-token ./dev preset "${BUILD_PRESET}-nothreads-all-tests" --configure-only \
			-DTAU_BUILD_JOBS=${BUILD_JOBS}; \
	fi

# Build the tree configured above, then run the node suites and the parity
# check (if TESTS = "yes"). The native tau the parity check compares against
# was built in the resolve stage and is inherited here.
FROM wasm-node-resolve AS wasm-node

ARG BUILD_JOBS=5
ARG BUILD_PRESET=release-wasm
ARG TESTS=yes

# The -all-tests preset builds the library, the suite and the CLI from one
# configure, so the fetched dependencies compile once.
RUN --mount=type=secret,id=gh_token \
	echo "(BUILD) -- Building wasm: tau.js/tau.wasm/tau.esm.mjs" && \
	echo " (BUILD) -- Running node tests: $TESTS" && \
	if [ "$TESTS" = "yes" ]; then \
		scripts/with-gh-token ./dev preset ${BUILD_PRESET}-all-tests \
			-DTAU_BUILD_JOBS=${BUILD_JOBS} -DTAU_PARITY_REQUIRE_NATIVE=ON; \
	else \
		scripts/with-gh-token ./dev preset ${BUILD_PRESET} -DTAU_BUILD_JOBS=${BUILD_JOBS}; \
	fi

RUN --mount=type=secret,id=gh_token \
	if [ "$TESTS" = "yes" ]; then \
	echo "(BUILD) -- Building wasm no-thread tests" && \
	scripts/with-gh-token ./dev preset "${BUILD_PRESET}-nothreads-all-tests" -DTAU_BUILD_JOBS=${BUILD_JOBS}; \
fi

RUN if [ "$TESTS" = "yes" ]; then \
	echo "(BUILD) -- Smoke-testing tau.node.js" && \
	node build/${BUILD_PRESET}/tau.node.js; \
fi

RUN if [ "$TESTS" = "yes" ]; then \
	echo "(BUILD) -- Running wasm node tests: ${BUILD_PRESET}-all-tests" && \
	ctest --preset ${BUILD_PRESET}-all-tests -j ${BUILD_JOBS} --output-on-failure; \
fi

RUN if [ "$TESTS" = "yes" ]; then \
	echo "(BUILD) -- Running wasm no-thread tests" && \
	ctest --preset "${BUILD_PRESET}-nothreads-all-tests" -j ${BUILD_JOBS} --output-on-failure; \
fi

# parity.js compares the wasm module against the native tau built above; the
# js_parity ctest entry in ${BUILD_PRESET}-all-tests runs it. TAU_WASM_JS names
# this preset's module, so a non-default BUILD_PRESET does not fall back to
# parity.js's own build/release-wasm/tau.js default when run by hand.
ENV TAU_NATIVE_BIN=/tau-lang/build/release/tau
ENV TAU_WASM_JS=/tau-lang/build/${BUILD_PRESET}/tau.js

RUN if [ "$TESTS" = "yes" ]; then \
	echo "(BUILD) -- Re-running wasm vs native parity check" && \
	node bindings/js/tests/parity.js; \
fi


# ------------------------------------------------------------
# WebAssembly browser dependencies: Chrome for Testing, puppeteer-core and
# the xterm.js packages js/repl vendors into its page.

FROM wasm-deps AS wasm-browser-deps

# Chrome's shared-library dependencies, needed only by the browser layer.
RUN apt-get update && apt-get install -y --no-install-recommends \
	libnss3 libnspr4 libatk1.0-0 libatk-bridge2.0-0 libcups2 libdrm2 \
	libdbus-1-3 libxkbcommon0 libxcomposite1 libxdamage1 libxfixes3 \
	libxrandr2 libgbm1 libasound2t64 libpango-1.0-0 libcairo2 libx11-6 \
	libx11-xcb1 libxcb1 libxext6 libxshmfence1 libglib2.0-0 fonts-liberation

# Only the files these installs read, so a source change keeps their layers.
COPY ./scripts/dep-chrome.sh ./scripts/dep-js-test-deps.sh /tau-lang/scripts/
COPY ./bindings/js/package.json ./bindings/js/package-lock.json \
	/tau-lang/bindings/js/

# Chrome for Testing, pinned separately from puppeteer-core's own version.
RUN echo "(BUILD) -- Installing Chrome for Testing" && \
	cd /tau-lang && \
	./dev dep-chrome.sh

# puppeteer-core for the headless-Chrome harness, and the xterm.js packages
# js/repl copies into its page.
RUN echo "(BUILD) -- Installing JS browser-test devDependencies" && \
	cd /tau-lang && \
	./dev dep-js-test-deps.sh

# Chrome refuses to start as root, which is the only user in this image.
ENV TAU_CHROME_ARGS=--no-sandbox


# ------------------------------------------------------------
# WebAssembly browser gate: the FTXUI browser REPL page, the compiled suite in
# headless Chrome (browser_suite) and the REPL page start check.

FROM wasm-browser-deps AS wasm-browser

COPY --from=source /tau-lang /tau-lang

# The wasm-node store entries, so the browser configure resolves nothing again.
# This belongs to the browser stage and not to wasm-browser-deps: from there the
# wasm-deps job would pull the whole wasm-node-resolve build in behind it.
COPY --from=wasm-node-resolve /root/.tau/store /root/.tau/store

WORKDIR /tau-lang

# The build context carries no .git, so the stamp arrives as a build argument.
ARG TAU_GIT_DESCRIBED=
ARG TAU_GIT_BRANCH=
ARG TAU_GIT_COMMIT_HASH=
ARG TAU_PARSER_GIT_DESCRIBED=
ARG TAU_PARSER_GIT_BRANCH=
ARG TAU_PARSER_GIT_COMMIT_HASH=
ARG TAU_PARSER_COMMIT=
ENV TAU_GIT_DESCRIBED=${TAU_GIT_DESCRIBED} \
	TAU_GIT_BRANCH=${TAU_GIT_BRANCH} \
	TAU_GIT_COMMIT_HASH=${TAU_GIT_COMMIT_HASH} \
	TAU_PARSER_GIT_DESCRIBED=${TAU_PARSER_GIT_DESCRIBED} \
	TAU_PARSER_GIT_BRANCH=${TAU_PARSER_GIT_BRANCH} \
	TAU_PARSER_GIT_COMMIT_HASH=${TAU_PARSER_GIT_COMMIT_HASH} \
	TAU_PARSER_COMMIT=${TAU_PARSER_COMMIT}

ARG BUILD_JOBS=5

# Argument BUILD_PRESET=debug-wasm picks the debugging preset family
ARG BUILD_PRESET=release-wasm

# Argument TESTS=no builds only the library, skipping the suite entirely
ARG TESTS=yes

# The remote store: configure reads a missing wasm package from it before it
# builds one (cmake/tau-deps.cmake).
ARG TAU_STORE_REMOTE=
ENV TAU_STORE_REMOTE=${TAU_STORE_REMOTE}

# TAU_BUILD_BROWSER_TESTS adds the browser_suite ctest entry, which runs the
# compiled suite in headless Chrome through
# bindings/js/tests/run-suite-in-chrome.js. This stage does not build the native
# tau the js_parity gate needs, so it turns that gate off.
RUN --mount=type=secret,id=gh_token \
	if [ "$TESTS" = "yes" ]; then \
	scripts/with-gh-token ./dev preset ${BUILD_PRESET}-all-tests -DTAU_BUILD_JOBS=${BUILD_JOBS} \
		-DTAU_BUILD_BROWSER_TESTS=ON -DTAU_PARITY_REQUIRE_NATIVE=OFF; \
fi

RUN if [ "$TESTS" = "yes" ]; then \
	echo "(BUILD) -- Running wasm browser tests: ${BUILD_PRESET}-all-tests" && \
	ctest --preset ${BUILD_PRESET}-all-tests -j ${BUILD_JOBS} --output-on-failure; \
fi

# The FTXUI REPL page (tau_repl_web.js/.wasm and vendor/) plus the REPL suite
# replayed inside it in headless Chrome. The suite reads its case list from
# the node build configured above (build/release-wasm).
RUN --mount=type=secret,id=gh_token \
	if [ "$TESTS" = "yes" ]; then \
	echo "(BUILD) -- Running the REPL suite inside the browser REPL" && \
	scripts/with-gh-token ./dev preset ${BUILD_PRESET}-repl-tests-browser run -DTAU_BUILD_JOBS=${BUILD_JOBS}; \
fi

RUN if [ "$TESTS" = "yes" ]; then \
	echo "(BUILD) -- Checking the browser REPL page in Chrome" && \
	node bindings/js/tests/repl-browser-check.js build/${BUILD_PRESET}-repl-browser; \
fi
