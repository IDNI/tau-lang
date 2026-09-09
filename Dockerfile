# This is a Dockerfile for building, testing, packaging and running the tau-lang

# To build the image with tag 'tau' from ./Dockerfile:
#   docker build --target runner -t tau .

# This Dockerfile contains the following stages (--target <stage>):
# - base: contains the base system with installed system dependencies
# - Linux branch is:
#   - deps: dependencies (cvc5)
#   - build: builds tau executable and runs tests
#   - packages: creates a release packages (deb and rpm)
#   - runner: provides a deb based image with installed tau package
#   - rpm-runner: provides an rpm based image with installed tau package
# - Windows branch is:
#   - w64-deps: dependencies built with mingw-w64 (cvc5 and boost)
#   - w64-build: builds tau executable for Windows
#   - w64-packages: creates a release packages (installer and zip file)

# use --build-arg BUILD_JOBS=N to set the number of build jobs (default is 5, 0 is for half of the available logical CPU cores)
# use --build-arg BUILD_PRESET="debug" for building of the debugging version (build stage)
# use --build-arg TESTS="no" to skip running tests (build stage)
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
FROM ubuntu:24.04 AS base

# Install dependencies
RUN echo "(BUILD) -- Installing dependencies" && \
	apt-get update && apt-get install -y \
	bash wget git nsis rpm python3-pip python3-venv bison nanobind-dev \
	cmake=3.28.3-1build7 \
	g++=4:13.2.0-7ubuntu1 \
	mingw-w64=11.0.1-3build1 \
	libboost-all-dev=1.83.0.1ubuntu2 \
	clang=1:18.0-59~exp2

# create tau-lang directory and set it as the working directory
RUN echo "(BUILD) -- Creating /tau-lang and home directory" && \
	mkdir -p /tau-lang ~/.tau


# ------------------------------------------------------------
# Source stage to copy from (in order to COPY from building host only once)

FROM base AS source

# Copy source files to tau-lang directory
COPY ./.git      /tau-lang/.git
COPY ./bindings  /tau-lang/bindings
COPY ./cmake     /tau-lang/cmake
COPY ./external  /tau-lang/external
COPY ./licenses  /tau-lang/licenses
COPY ./scripts   /tau-lang/scripts
COPY ./src       /tau-lang/src
COPY ./tests     /tau-lang/tests
COPY ./.gitignore ./.gitmodules ./CMakeLists.txt ./dev \
	./README.md ./LICENSE.md ./VERSION      /tau-lang/
COPY ./parser/*.generated.h ./parser/*.generated.cpp   /tau-lang/parser/


# ============================================================

# ------------------------------------------------------------
# Linux dependencies image

FROM base AS deps

ARG BUILD_JOBS=5

COPY ./dev 			/tau-lang/
# ./dev needs devrc; copy only it so parser changes do not rebuild the deps
COPY ./external/parser/scripts/devrc 	/tau-lang/external/parser/scripts/
COPY ./scripts/with-gh-token	/tau-lang/scripts/
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

# *-all enables the executable and the tests in one configure
RUN echo "(BUILD) -- Building ${BUILD_PRESET} version: $(head -n 1 VERSION)" && \
	echo " (BUILD) -- Running tests: $TESTS" && \
	if [ "$TESTS" = "yes" ]; then \
		./dev preset ${BUILD_PRESET}-all run -DTAU_BUILD_JOBS=${BUILD_JOBS}; \
	else \
		./dev preset ${BUILD_PRESET}-tau -DTAU_BUILD_JOBS=${BUILD_JOBS}; \
	fi

# Set TEST_GCC_BUILD=no to skip the gcc compilation check
ARG TEST_GCC_BUILD=yes

# Presets build with clang; devel (-O0 -g0) checks the tree still builds with gcc
RUN if [ "$TESTS" = "yes" -a "$TEST_GCC_BUILD" = "yes" ]; then \
	./dev preset devel-ninja-gcc -DTAU_BUILD_JOBS=${BUILD_JOBS} && \
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
# ./dev needs devrc; copy only it so parser changes do not rebuild the deps
COPY ./external/parser/scripts/devrc 	/tau-lang/external/parser/scripts/
COPY ./scripts/with-gh-token	/tau-lang/scripts/
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

# Argument NIGHTLY=yes is used to build nightly packages (works only if RELEASE=yes)
ARG NIGHTLY=no

# if NIGHTLY is set to yes, then add .YYYY-MM-DD to the first line of the VERSION file
RUN if [ "$NIGHTLY" = "yes" ]; then \
	echo -n "$(head -n 1 VERSION)-$(date --iso)" > VERSION; \
fi

ARG BUILD_JOBS=5

# Argument BUILD_PRESET=release/debug picks the CMake preset family
ARG BUILD_PRESET=release

# Build tau executable
RUN echo "(BUILD) -- Building w64 ${BUILD_PRESET} version: $(head -n 1 VERSION)" && \
	./dev preset ${BUILD_PRESET}-mingw -DTAU_BUILD_JOBS=${BUILD_JOBS} \
		-DTAU_BUILD_EXECUTABLE=ON

# TODO add tests for Windows build

# # Argument TESTS=no is used to skip running tests
# ARG TESTS=yes

# # Build tests and run them if TESTS="yes". Stop the build if they fail
# RUN echo " (BUILD) -- Running tests: $TESTS"
# RUN if [ "$TESTS" = "yes" ]; then \
# 	./dev preset ${BUILD_PRESET}-mingw run -DTAU_BUILD_JOBS=${BUILD_JOBS} \
# 		-DTAU_BUILD_TESTS=ON; \
# fi


# ------------------------------------------------------------
# create Windows packages

FROM w64-build AS w64-packages

ARG BUILD_JOBS=5

WORKDIR /tau-lang

RUN echo "(BUILD) -- Building Windows packages" && \
	./dev preset release-mingw-packages-zip -DTAU_BUILD_JOBS=${BUILD_JOBS} && \
	./dev preset release-mingw-packages -DTAU_BUILD_JOBS=${BUILD_JOBS}


