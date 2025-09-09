# This is a Dockerfile for building, testing, packaging and running the tau-lang

# To build the image with tag 'tau' from ./Dockerfile:
#   docker build -t tau .

# use --build-arg TESTS="no" to skip running tests
# use --build-arg BUILD_TYPE="Debug" for building of the debugging version

# To run tau using the created image in interactive mode:
#   docker run --rm -it tau [<tau options>]

# --rm flag is used to remove the container after it exits


FROM ubuntu:24.04

# Install dependencies
RUN apt-get update && apt-get install -y \
	bash wget git nsis rpm \
	cmake=3.28.3-1build7 \
	g++=4:13.2.0-7ubuntu1 \
	mingw-w64=11.0.1-3build1 \
	libboost-all-dev=1.83.0.1ubuntu2 \
	libz3-dev=4.8.12-3.1build1 \
	python3-distutils-extra \
	cvc5 \
	libcvc5-1 \
	libcvc5-dev \
	libcvc5parser1 \
	python3-venv \
	python3-pip

# Argument BUILD_TYPE=Debug/Release
ARG BUILD_TYPE=Release

# Argument RELEASE=yes is used to build release packages
ARG RELEASE=no

# Argument NIGHTLY=yes is used to build nightly packages (works only if RELEASE=yes)
ARG NIGHTLY=no

# Argument TESTS=no is used to skip running tests
ARG TESTS=yes

# Copy source code
COPY ./ /tau-lang

# # OR clone from git
# RUN git clone https://github.com/IDNI/tau-lang /tau-lang

WORKDIR /tau-lang

RUN ./dev clean

# if NIGHTLY is set to yes, then add .YYYY-MM-DD to the first line of the VERSION file
RUN if [ "$NIGHTLY" = "yes" ]; then \
	echo -n "$(head -n 1 VERSION)-$(date --iso)" > VERSION; \
fi
RUN echo "(BUILD) -- Building version: $(head -n 1 VERSION)"

# Build tests and run them if TESTS is set to yes. Stop the build if they fail
RUN echo " (BUILD) -- Running tests: $TESTS"
RUN if [ "$TESTS" = "yes" ]; then \
	./dev build "${BUILD_TYPE}" -DTAU_BUILD_TESTS=ON && \
	cd tests && \
	ctest -j 8 --test-dir "../build-${BUILD_TYPE}" --output-on-failure \
		|| exit 1; \
fi

RUN echo "(BUILD) -- Building packages: $RELEASE (nightly: $NIGHTLY)"

# Linux packages
RUN if [ "$RELEASE" = "yes" ]; then \
	./dev packages && rm ./build-Release/CMakeCache.txt; \
fi

# Windows packages
RUN if [ "$RELEASE" = "yes" ]; then \
	./dev boost-mingw && \
	./dev w64-packages; \
fi

# If tau executable does not exist already, build it
RUN if [ ! -f ./build-${BUILD_TYPE}/tau ]; then ./dev build "${BUILD_TYPE}"; fi

# Set the entrypoint to the tau executable
ENV LD_LIBRARY_PATH=/tau-lang/exteral/cvc5/build/src:$LD_LIBRARY_PATH
WORKDIR /tau-lang/build-${BUILD_TYPE}
ENTRYPOINT [ "./tau" ]
CMD []
