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
#   - wasm-deps: extends the native dependencies with emsdk
#   - wasm-node: builds tau.js/tau.wasm/tau.esm.mjs and (TESTS=yes) runs the node
#     suite, the no-thread configuration and the wasm-vs-native parity check
#   - wasm-browser-deps: adds Chrome for Testing, puppeteer-core and xterm.js
#   - wasm-browser: builds the browser REPL page and (TESTS=yes) runs the
#     compiled suite in headless Chrome, the REPL suite inside the page, and
#     the REPL page start check

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

# Dependencies resolve through the store at configure time
# (cmake/tau-deps.cmake), never a pre-source dist layer. This stage only
# installs oras, the pinned client configure uses to read a missing package
# from the remote store; dep-oras.sh verifies the release by sha256.
COPY ./dev /tau-lang/
COPY ./external/parser/scripts/devrc /tau-lang/external/parser/scripts/
COPY ./external/parser/cmake/tau-resolve.cmake /tau-lang/external/parser/cmake/
COPY ./scripts/env /tau-lang/scripts/
COPY ./scripts/dep-oras.sh /tau-lang/scripts/
RUN cd /tau-lang && ./dev dep-oras


# ------------------------------------------------------------
# Build tau executable and run tests (if TESTS = "yes")

FROM deps AS build

COPY --from=source /tau-lang /tau-lang

WORKDIR /tau-lang

# The build context carries no .git, so the stamp arrives as a build argument.
ARG TAU_GIT_DESCRIBED=
ARG TAU_GIT_BRANCH=
ARG TAU_GIT_COMMIT_HASH=
ARG TAU_PARSER_GIT_DESCRIBED=
ARG TAU_PARSER_GIT_BRANCH=
ARG TAU_PARSER_GIT_COMMIT_HASH=
ENV TAU_GIT_DESCRIBED=${TAU_GIT_DESCRIBED} \
	TAU_GIT_BRANCH=${TAU_GIT_BRANCH} \
	TAU_GIT_COMMIT_HASH=${TAU_GIT_COMMIT_HASH} \
	TAU_PARSER_GIT_DESCRIBED=${TAU_PARSER_GIT_DESCRIBED} \
	TAU_PARSER_GIT_BRANCH=${TAU_PARSER_GIT_BRANCH} \
	TAU_PARSER_GIT_COMMIT_HASH=${TAU_PARSER_GIT_COMMIT_HASH}

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

# *-all enables the executable and the tests in one configure
RUN --mount=type=cache,target=/root/.ccache,sharing=locked \
	--mount=type=secret,id=gh_token \
	echo "(BUILD) -- Building ${BUILD_PRESET} version: $(head -n 1 VERSION)" && \
	echo " (BUILD) -- Running tests: $TESTS" && \
	if [ "$TESTS" = "yes" ]; then \
		scripts/with-gh-token ./dev preset ${BUILD_PRESET}-all run -DTAU_BUILD_JOBS=${BUILD_JOBS} \
			${TAU_BAS:+-DTAU_BAS=${TAU_BAS}} \
			-DCMAKE_C_COMPILER_LAUNCHER=ccache \
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache; \
	else \
		scripts/with-gh-token ./dev preset ${BUILD_PRESET}-tau -DTAU_BUILD_JOBS=${BUILD_JOBS} \
			-DCMAKE_C_COMPILER_LAUNCHER=ccache \
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache; \
	fi && \
	ccache --show-stats

# The trusted workflow turns this on: the same image that resolves the store
# packages publishes the ones the remote lacks, so the Linux ids match what
# every Docker consumer requests. Off for every other build.
ARG TAU_STORE_PUBLISH=OFF
RUN --mount=type=secret,id=gh_token \
	if [ "$TAU_STORE_PUBLISH" = "ON" ]; then \
		scripts/with-gh-token ./dev store-publish; \
	fi

# Set TEST_GCC_BUILD=no to skip the gcc compilation check
ARG TEST_GCC_BUILD=yes

# Check also make and gcc compilation since ninja and clang is used by default
RUN --mount=type=cache,target=/root/.ccache,sharing=locked \
	--mount=type=secret,id=gh_token \
	if [ "$TESTS" = "yes" -a "$TEST_GCC_BUILD" = "yes" ]; then \
	scripts/with-gh-token ./dev preset devel-make-gcc -DTAU_BUILD_JOBS=${BUILD_JOBS} \
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

FROM deps AS w64-deps

# The w64 packages have their own content ids (target=w64) in the same store
# as the native ones, and oras is already installed in the parent stage. This
# stage exists so the w64 image stays a separate, cacheable step from the
# Linux one; configure builds a w64 package the remote does not have.
ARG BUILD_JOBS=5


# ------------------------------------------------------------
# Windows build image

FROM w64-deps AS w64-build

COPY --from=source /tau-lang /tau-lang

WORKDIR /tau-lang

# The build context carries no .git, so the stamp arrives as a build argument.
ARG TAU_GIT_DESCRIBED=
ARG TAU_GIT_BRANCH=
ARG TAU_GIT_COMMIT_HASH=
ARG TAU_PARSER_GIT_DESCRIBED=
ARG TAU_PARSER_GIT_BRANCH=
ARG TAU_PARSER_GIT_COMMIT_HASH=
ENV TAU_GIT_DESCRIBED=${TAU_GIT_DESCRIBED} \
	TAU_GIT_BRANCH=${TAU_GIT_BRANCH} \
	TAU_GIT_COMMIT_HASH=${TAU_GIT_COMMIT_HASH} \
	TAU_PARSER_GIT_DESCRIBED=${TAU_PARSER_GIT_DESCRIBED} \
	TAU_PARSER_GIT_BRANCH=${TAU_PARSER_GIT_BRANCH} \
	TAU_PARSER_GIT_COMMIT_HASH=${TAU_PARSER_GIT_COMMIT_HASH}

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

# WINEPREFIX keeps the wine configuration out of the home directory.
# WINEDEBUG drops wine's own noise, and not the output of a test.
ENV WINEPREFIX=/root/.wine-tau WINEDEBUG=-all

# Build tau executable, and run its suite under wine if TESTS = "yes"
# wine, not wine64: Ubuntu's wine package runs 64-bit PE executables on its
# own, with no i386 multiarch needed for these mingw-w64-x86_64 binaries
RUN --mount=type=secret,id=gh_token \
	echo "(BUILD) -- Building w64 ${BUILD_PRESET} version: $(head -n 1 VERSION)" && \
	echo " (BUILD) -- Running tests: $TESTS" && \
	scripts/with-gh-token ./dev preset ${BUILD_PRESET}-w64 -DTAU_BUILD_JOBS=${BUILD_JOBS} \
		-DTAU_BUILD_EXECUTABLE=ON && \
	if [ "$TESTS" = "yes" ]; then \
		apt-get update && apt-get install -y --no-install-recommends wine && \
		scripts/with-gh-token ./dev preset ${BUILD_PRESET}-w64 -DTAU_BUILD_JOBS=${BUILD_JOBS} \
			-DTAU_BUILD_TESTS=ON -DCMAKE_CROSSCOMPILING_EMULATOR=wine && \
		ctest --test-dir build/${BUILD_PRESET}-w64 -j ${BUILD_JOBS} \
			--output-on-failure; \
	fi

# The trusted workflow turns this on to publish the w64 packages it just built.
ARG TAU_STORE_PUBLISH=OFF
RUN --mount=type=secret,id=gh_token \
	if [ "$TAU_STORE_PUBLISH" = "ON" ]; then \
		scripts/with-gh-token ./dev store-publish; \
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
RUN ln -s "$(ls -d /root/.tau/emsdk/node/*/bin | head -n1)/node" /usr/local/bin/node


# ------------------------------------------------------------
# WebAssembly Node.js gate: build tau.js/tau.wasm/tau.esm.mjs and (with
# TESTS=yes) run the wasm suite under emsdk's node, the no-thread
# configuration, and the wasm-vs-native parity check. No Chrome or puppeteer.

FROM wasm-deps AS wasm-node

COPY --from=source /tau-lang /tau-lang

WORKDIR /tau-lang

# The build context carries no .git, so the stamp arrives as a build argument.
ARG TAU_GIT_DESCRIBED=
ARG TAU_GIT_BRANCH=
ARG TAU_GIT_COMMIT_HASH=
ARG TAU_PARSER_GIT_DESCRIBED=
ARG TAU_PARSER_GIT_BRANCH=
ARG TAU_PARSER_GIT_COMMIT_HASH=
ENV TAU_GIT_DESCRIBED=${TAU_GIT_DESCRIBED} \
	TAU_GIT_BRANCH=${TAU_GIT_BRANCH} \
	TAU_GIT_COMMIT_HASH=${TAU_GIT_COMMIT_HASH} \
	TAU_PARSER_GIT_DESCRIBED=${TAU_PARSER_GIT_DESCRIBED} \
	TAU_PARSER_GIT_BRANCH=${TAU_PARSER_GIT_BRANCH} \
	TAU_PARSER_GIT_COMMIT_HASH=${TAU_PARSER_GIT_COMMIT_HASH}

ARG BUILD_JOBS=5

# Argument BUILD_PRESET=debug-wasm picks the debugging preset family
ARG BUILD_PRESET=release-wasm

# Argument TESTS=no builds only the library, skipping the suite entirely
ARG TESTS=yes

# The remote store: configure reads a missing wasm package from it before it
# builds one (cmake/tau-deps.cmake).
ARG TAU_STORE_REMOTE=
ENV TAU_STORE_REMOTE=${TAU_STORE_REMOTE}

# Native tau for the parity check, built before the wasm configure: js_parity
# is registered only when TAU_PARITY_NATIVE_BIN exists (tests/CMakeLists.txt).
RUN --mount=type=secret,id=gh_token \
	if [ "$TESTS" = "yes" ]; then \
	echo "(BUILD) -- Building native tau (sbf,tau pack) for parity" && \
	scripts/with-gh-token ./dev preset release-tau -DTAU_BAS=sbf,tau -DTAU_BUILD_JOBS=${BUILD_JOBS}; \
	fi

# The -all-tests preset builds the library, the suite and the CLI from one
# configure, so the fetched dependencies compile once. This stage builds the
# native tau the prescribed js_parity gate compares against, so it requires it.
RUN --mount=type=secret,id=gh_token \
	echo "(BUILD) -- Building wasm: tau.js/tau.wasm/tau.esm.mjs" && \
	echo " (BUILD) -- Running node tests: $TESTS" && \
	if [ "$TESTS" = "yes" ]; then \
		scripts/with-gh-token ./dev preset ${BUILD_PRESET}-all-tests \
			-DTAU_BUILD_JOBS=${BUILD_JOBS} -DTAU_PARITY_REQUIRE_NATIVE=ON; \
	else \
		scripts/with-gh-token ./dev preset ${BUILD_PRESET} -DTAU_BUILD_JOBS=${BUILD_JOBS}; \
	fi

# The trusted workflow turns this on to publish the wasm packages it just built.
ARG TAU_STORE_PUBLISH=OFF
RUN --mount=type=secret,id=gh_token \
	if [ "$TAU_STORE_PUBLISH" = "ON" ]; then \
		scripts/with-gh-token ./dev store-publish; \
	fi

RUN if [ "$TESTS" = "yes" ]; then \
	echo "(BUILD) -- Smoke-testing tau.node.js" && \
	node build/${BUILD_PRESET}/tau.node.js; \
fi

RUN if [ "$TESTS" = "yes" ]; then \
	echo "(BUILD) -- Running wasm node tests: ${BUILD_PRESET}-all-tests" && \
	ctest --preset ${BUILD_PRESET}-all-tests -j ${BUILD_JOBS} --output-on-failure; \
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

WORKDIR /tau-lang

# The build context carries no .git, so the stamp arrives as a build argument.
ARG TAU_GIT_DESCRIBED=
ARG TAU_GIT_BRANCH=
ARG TAU_GIT_COMMIT_HASH=
ARG TAU_PARSER_GIT_DESCRIBED=
ARG TAU_PARSER_GIT_BRANCH=
ARG TAU_PARSER_GIT_COMMIT_HASH=
ENV TAU_GIT_DESCRIBED=${TAU_GIT_DESCRIBED} \
	TAU_GIT_BRANCH=${TAU_GIT_BRANCH} \
	TAU_GIT_COMMIT_HASH=${TAU_GIT_COMMIT_HASH} \
	TAU_PARSER_GIT_DESCRIBED=${TAU_PARSER_GIT_DESCRIBED} \
	TAU_PARSER_GIT_BRANCH=${TAU_PARSER_GIT_BRANCH} \
	TAU_PARSER_GIT_COMMIT_HASH=${TAU_PARSER_GIT_COMMIT_HASH}

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
