FROM ubuntu:24.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    bash cmake git g++ mingw-w64 libboost-all-dev nsis rpm wget

# Argument BUILD_TYPE=Debug/Release
ARG BUILD_TYPE=Release

# Argument RELEASE=yes is used to build release packages
ARG RELEASE=no

# Argument NIGHTLY=yes is used to build nightly packages
ARG NIGHTLY=no

# Argument TESTS=no is used to skip running tests
ARG TESTS=yes

# Copy source code
COPY ./ /tau-lang

# # OR clone from git
# RUN git clone https://github.com/IDNI/tau-lang /tau-lang

WORKDIR /tau-lang

# if NIGHTLY is set to yes, then add .YYYY-MM-DD to the first line of the VERSION file
RUN if [ "$NIGHTLY" = "yes" ]; then \
	echo "$(head -n 1 VERSION)-$(date --iso)" > VERSION; \
fi
RUN echo "(BUILD) -- Building version: $(cat VERSION)"

# Build tests and run them if TESTS is set to yes. Stop the build if they fail
RUN echo " (BUILD) -- Running tests: $TESTS"
RUN if [ "$TESTS" = "yes" ]; then \
	./build.sh "${BUILD_TYPE}" -DTAU_BUILD_TESTS=ON && \
	cd tests && \
	ctest -j 8 --test-dir "../build-${BUILD_TYPE}" --output-on-failure \
		|| exit 1; \
fi

RUN if [ "$NIGHTLY" = "yes" ]; then RELEASE=yes; fi
RUN echo "(BUILD) -- Building packages: $RELEASE (nightly: $NIGHTLY)"

# Linux packages
RUN if [ "$RELEASE" = "yes" ]; then ./packages.sh; fi

# Windows packages
RUN if [ "$RELEASE" = "yes" ]; then \
	cd external && ./libboost-mingw-builder.sh && cd .. \
	./w64-packages.sh; \
fi
