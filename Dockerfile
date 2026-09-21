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
#   - runner: provides a deb based image with installed tau package
#   - rpm-runner: provides an rpm based image with installed tau package
# - Windows branch is:
#   - w64-deps: dependencies built with mingw-w64 (cvc5 and boost)
#   - w64-build: builds tau executable for Windows, and (TESTS=yes) runs its suite under wine
#   - w64-packages: creates a release packages (installer and zip file)
# - WebAssembly branch is (see AGENTS.md's WebAssembly section for the constraints):
#   - wasm-deps: extends the native dependencies with emsdk and boost for emscripten
#   - wasm-build: builds tau.js/tau.wasm/tau.esm.mjs, and (TESTS=yes) the wasm suite

# use --build-arg BUILD_JOBS=N to set the number of build jobs (default is 5, 0 is for half of the available logical CPU cores)
# use --build-arg BUILD_PRESET="debug" for building of the debugging version (build stage)
# use --build-arg TESTS="no" to skip running tests (build, w64-build, wasm-build)
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
RUN echo "(BUILD) -- Installing dependencies" && \
	apt-get update && apt-get install -y \
	bash wget git gnupg nsis rpm ninja-build bison ccache curl unzip \
	python3-pip python3-venv python3-dev nanobind-dev \
	cmake=3.28.3-1build7 \
	g++=4:13.2.0-7ubuntu1 \
	mingw-w64=11.0.1-3build1 \
	libboost-all-dev=1.83.0.1ubuntu2 \
	clang-19=1:19.1.1-1ubuntu1~24.04.2

# spot gives ltlsynt and ltl2tgba to the LTL suites
RUN echo "(BUILD) -- Installing spot" && \
	wget -q -O - 'https://build.opensuse.org/projects/home:adl/signing_keys/download?kind=gpg' \
		| gpg --dearmor -o /usr/share/keyrings/home-adl-obs.gpg && \
	echo 'deb [signed-by=/usr/share/keyrings/home-adl-obs.gpg] https://download.opensuse.org/repositories/home:/adl/xUbuntu_24.04/ ./' \
		> /etc/apt/sources.list.d/home-adl-obs.list && \
	apt-get update && apt-get install -y --no-install-recommends spot

# create tau-lang directory and set it as the working directory
RUN echo "(BUILD) -- Creating /tau-lang and home directory" && \
	mkdir -p /tau-lang ~/.tau


# ------------------------------------------------------------
# Source stage to copy from (in order to COPY from building host only once)

FROM base AS source

# Copy source files to tau-lang directory
COPY ./bindings  /tau-lang/bindings
COPY ./cmake     /tau-lang/cmake
COPY ./external  /tau-lang/external
COPY ./licenses  /tau-lang/licenses
COPY ./scripts   /tau-lang/scripts
COPY ./src       /tau-lang/src
COPY ./tests     /tau-lang/tests
COPY ./.gitignore ./.gitmodules ./CMakeLists.txt ./CMakePresets.json ./dev \
	./README.md ./LICENSE.md ./VERSION      /tau-lang/
COPY ./parser   /tau-lang/parser


# ============================================================

# ------------------------------------------------------------
# Linux dependencies image

FROM base AS deps

ARG BUILD_JOBS=5

COPY ./dev 			/tau-lang/
COPY ./external/parser/scripts/devrc 	/tau-lang/external/parser/scripts/
COPY ./external/parser/cmake/tau-resolve.cmake 	/tau-lang/external/parser/cmake/
COPY ./scripts/with-gh-token	/tau-lang/scripts/
COPY ./scripts/env		/tau-lang/scripts/
COPY ./scripts/dep-cvc5.sh	/tau-lang/scripts/
RUN --mount=type=secret,id=gh_token \
	echo "(BUILD) -- Building dependencies: cvc5" && \
	cd /tau-lang && \
	scripts/with-gh-token ./dev dep-cvc5.sh -DTAU_BUILD_JOBS=${BUILD_JOBS}
COPY ./scripts/dep-boost.sh	/tau-lang/scripts/
RUN --mount=type=secret,id=gh_token \
	echo "(BUILD) -- Building dependencies: boost" && \
	cd /tau-lang && \
	scripts/with-gh-token ./dev dep-boost -DTAU_BUILD_JOBS=${BUILD_JOBS}


# ------------------------------------------------------------
# Build tau executable and run tests (if TESTS = "yes")

FROM deps AS build

COPY --from=source /tau-lang /tau-lang

WORKDIR /tau-lang

# The build context carries no .git, so the stamp arrives as a build argument.
ARG TAU_GIT_DESCRIBED=
ARG TAU_GIT_BRANCH=
ARG TAU_GIT_COMMIT_HASH=
ENV TAU_GIT_DESCRIBED=${TAU_GIT_DESCRIBED} \
	TAU_GIT_BRANCH=${TAU_GIT_BRANCH} \
	TAU_GIT_COMMIT_HASH=${TAU_GIT_COMMIT_HASH}

# The presets name clang and clang++. The versioned package provides
# only clang-19 and clang++-19.
RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-19 100 && \
	update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-19 100

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

# Argument TAU_BAS=<ids> picks the pack; empty keeps the default pack
ARG TAU_BAS=

# The source COPY above changes on every commit, so no layer cache can hold
# the objects. A ccache mount survives it, and it outlives the build.
ENV CCACHE_DIR=/root/.ccache CCACHE_MAXSIZE=3G

# *-all enables the executable and the tests in one configure
RUN --mount=type=cache,target=/root/.ccache,sharing=locked \
	echo "(BUILD) -- Building ${BUILD_PRESET} version: $(head -n 1 VERSION)" && \
	echo " (BUILD) -- Running tests: $TESTS" && \
	if [ "$TESTS" = "yes" ]; then \
		./dev preset ${BUILD_PRESET}-all run -DTAU_BUILD_JOBS=${BUILD_JOBS} \
			${TAU_BAS:+-DTAU_BAS=${TAU_BAS}} \
			-DCMAKE_C_COMPILER_LAUNCHER=ccache \
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache; \
	else \
		./dev preset ${BUILD_PRESET}-tau -DTAU_BUILD_JOBS=${BUILD_JOBS} \
			-DCMAKE_C_COMPILER_LAUNCHER=ccache \
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache; \
	fi && \
	ccache --show-stats

# Set TEST_GCC_BUILD=no to skip the gcc compilation check
ARG TEST_GCC_BUILD=yes

# Check also make and gcc compilation since ninja and clang is used by default
RUN --mount=type=cache,target=/root/.ccache,sharing=locked \
	if [ "$TESTS" = "yes" -a "$TEST_GCC_BUILD" = "yes" ]; then \
	./dev preset devel-make-gcc -DTAU_BUILD_JOBS=${BUILD_JOBS} \
		-DCMAKE_C_COMPILER_LAUNCHER=ccache \
		-DCMAKE_CXX_COMPILER_LAUNCHER=ccache && \
	rm -rf build/devel; \
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

FROM build AS testnet

ARG BUILD_JOBS=5

# Argument BUILD_PRESET=release/debug picks the CMake preset family
ARG BUILD_PRESET=release

WORKDIR /tau-lang

# The tau-testnet requirements build fastecdsa from source, which needs gmp
RUN apt-get update && apt-get install -y --no-install-recommends libgmp-dev

RUN --mount=type=cache,target=/root/.ccache,sharing=locked \
	--mount=type=secret,id=gh_token \
	scripts/with-gh-token ./dev test-with-tau-testnet \
		${BUILD_PRESET}-binding-python \
		-DTAU_BUILD_JOBS=${BUILD_JOBS} \
		-DCMAKE_C_COMPILER_LAUNCHER=ccache \
		-DCMAKE_CXX_COMPILER_LAUNCHER=ccache


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

FROM base AS w64-deps

ARG BUILD_JOBS=5

COPY ./dev			/tau-lang/
# ./dev needs devrc, and devrc resolves the prefix and the job count through
# tau-resolve.cmake; copy only those two so parser changes do not rebuild deps
COPY ./external/parser/scripts/devrc 	/tau-lang/external/parser/scripts/
COPY ./external/parser/cmake/tau-resolve.cmake 	/tau-lang/external/parser/cmake/
COPY ./scripts/with-gh-token	/tau-lang/scripts/
COPY ./scripts/env		/tau-lang/scripts/
COPY ./scripts/dep-cvc5.sh	/tau-lang/scripts/
RUN --mount=type=secret,id=gh_token \
	echo "(BUILD) -- Building w64 dependencies: cvc5" && \
	cd /tau-lang && \
	scripts/with-gh-token ./dev dep-cvc5 -DTAU_BUILD_JOBS=${BUILD_JOBS} --w64

COPY ./scripts/dep-boost.sh	/tau-lang/scripts/
RUN --mount=type=secret,id=gh_token \
	echo "(BUILD) -- Building w64 dependencies: boost" && \
	cd /tau-lang && \
	scripts/with-gh-token ./dev dep-boost -DTAU_BUILD_JOBS=${BUILD_JOBS} --w64


# ------------------------------------------------------------
# Windows build image

FROM w64-deps AS w64-build

COPY --from=source /tau-lang /tau-lang

WORKDIR /tau-lang

# The build context carries no .git, so the stamp arrives as a build argument.
ARG TAU_GIT_DESCRIBED=
ARG TAU_GIT_BRANCH=
ARG TAU_GIT_COMMIT_HASH=
ENV TAU_GIT_DESCRIBED=${TAU_GIT_DESCRIBED} \
	TAU_GIT_BRANCH=${TAU_GIT_BRANCH} \
	TAU_GIT_COMMIT_HASH=${TAU_GIT_COMMIT_HASH}

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

# WINEPREFIX keeps the wine configuration out of the home directory.
# WINEDEBUG drops wine's own noise, and not the output of a test.
ENV WINEPREFIX=/root/.wine-tau WINEDEBUG=-all

# Build tau executable, and run its suite under wine if TESTS = "yes"
# wine, not wine64: Ubuntu's wine package runs 64-bit PE executables on its
# own, with no i386 multiarch needed for these mingw-w64-x86_64 binaries
RUN echo "(BUILD) -- Building w64 ${BUILD_PRESET} version: $(head -n 1 VERSION)" && \
	echo " (BUILD) -- Running tests: $TESTS" && \
	./dev preset ${BUILD_PRESET}-mingw -DTAU_BUILD_JOBS=${BUILD_JOBS} \
		-DTAU_BUILD_EXECUTABLE=ON && \
	if [ "$TESTS" = "yes" ]; then \
		apt-get update && apt-get install -y --no-install-recommends wine && \
		./dev preset ${BUILD_PRESET}-mingw -DTAU_BUILD_JOBS=${BUILD_JOBS} \
			-DTAU_BUILD_TESTS=ON -DCMAKE_CROSSCOMPILING_EMULATOR=wine && \
		ctest --test-dir build/${BUILD_PRESET}-mingw -j ${BUILD_JOBS} \
			--output-on-failure; \
	fi


# ------------------------------------------------------------
# create Windows packages

FROM w64-build AS w64-packages

ARG BUILD_JOBS=5

WORKDIR /tau-lang

RUN echo "(BUILD) -- Building Windows packages" && \
	./dev preset release-mingw-packages-zip -DTAU_BUILD_JOBS=${BUILD_JOBS} && \
	./dev preset release-mingw-packages -DTAU_BUILD_JOBS=${BUILD_JOBS}


# ============================================================

# ------------------------------------------------------------
# WebAssembly dependencies image (emsdk and boost, no cvc5: the wasm pack
# is sbf,tau,qint,qlt)

FROM deps AS wasm-deps

ARG BUILD_JOBS=5

# ./dev, devrc, tau-resolve.cmake, with-gh-token, env and dep-boost.sh are
# already in place from the deps stage; only emsdk is new here.
COPY ./external/parser/scripts/dep-emsdk.sh 	/tau-lang/external/parser/scripts/
COPY ./scripts/dep-emsdk.sh	/tau-lang/scripts/
RUN echo "(BUILD) -- Building wasm dependencies: emsdk" && \
	cd /tau-lang && \
	./dev dep-emsdk.sh -DTAU_BUILD_JOBS=${BUILD_JOBS}
RUN --mount=type=secret,id=gh_token \
	echo "(BUILD) -- Building wasm dependencies: boost" && \
	cd /tau-lang && \
	scripts/with-gh-token ./dev dep-boost -DTAU_BUILD_JOBS=${BUILD_JOBS} --emscripten

# emsdk ships the only node in the image, under a version directory whose name
# is not fixed. tests/CMakeLists.txt requires node on the path under Emscripten.
RUN ln -s "$(ls -d /root/.tau/emsdk/node/*/bin | head -n1)/node" /usr/local/bin/node


# ------------------------------------------------------------
# WebAssembly build image: tau.js / tau.wasm / tau.esm.mjs, and (if TESTS=yes)
# builds and runs the wasm test suite, the browser suite and the parity check

FROM wasm-deps AS wasm-build

COPY --from=source /tau-lang /tau-lang

WORKDIR /tau-lang

# The build context carries no .git, so the stamp arrives as a build argument.
ARG TAU_GIT_DESCRIBED=
ARG TAU_GIT_BRANCH=
ARG TAU_GIT_COMMIT_HASH=
ENV TAU_GIT_DESCRIBED=${TAU_GIT_DESCRIBED} \
	TAU_GIT_BRANCH=${TAU_GIT_BRANCH} \
	TAU_GIT_COMMIT_HASH=${TAU_GIT_COMMIT_HASH}

ARG BUILD_JOBS=5

# Argument BUILD_PRESET=debug-emscripten picks the debugging preset family
ARG BUILD_PRESET=emscripten

# Argument TESTS=no builds only the library, skipping the suite entirely
ARG TESTS=yes

# The -all preset builds the library and the suite from one configure, so the
# fetched dependencies compile once. It also registers the browser_suite entry.
RUN echo "(BUILD) -- Building wasm: tau.js/tau.wasm/tau.esm.mjs" && \
	echo " (BUILD) -- Running tests: $TESTS" && \
	if [ "$TESTS" = "yes" ]; then \
		./dev preset ${BUILD_PRESET}-all -DTAU_BUILD_JOBS=${BUILD_JOBS} \
			-DTAU_BUILD_BROWSER_TESTS=ON; \
	else \
		./dev preset ${BUILD_PRESET} -DTAU_BUILD_JOBS=${BUILD_JOBS}; \
	fi

ENV TAU_NATIVE_BIN=/tau-lang/build/release/tau

# Chrome refuses to start as root, which is the only user in this image
ENV TAU_CHROME_ARGS=--no-sandbox

RUN if [ "$TESTS" = "yes" ]; then \
	echo "(BUILD) -- Smoke-testing tau.node.js" && \
	node build/${BUILD_PRESET}/tau.node.js; \
fi

RUN if [ "$TESTS" = "yes" ]; then \
	echo "(BUILD) -- Running wasm tests: ${BUILD_PRESET}-all" && \
	ctest --preset ${BUILD_PRESET}-all -j ${BUILD_JOBS} --output-on-failure; \
fi

# parity.js compares the wasm module against a native tau built from the
# identical pack. sbf,tau is the wasm pack's own base.
RUN if [ "$TESTS" = "yes" ]; then \
	echo "(BUILD) -- Building native tau (sbf,tau pack) for parity" && \
	./dev preset release-tau -DTAU_BAS=sbf,tau -DTAU_BUILD_JOBS=${BUILD_JOBS}; \
fi

RUN if [ "$TESTS" = "yes" ]; then \
	echo "(BUILD) -- Running wasm vs native parity check" && \
	node bindings/js/tests/parity.js; \
fi
