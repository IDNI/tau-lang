FROM ubuntu:24.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    bash cmake git g++ mingw-w64 libboost-all-dev nsis rpm

# Argument NIGHTLY=yes is used to determine if the build is a nightly build
ARG NIGHTLY=no

# Argument TESTS=no is used to skip running tests
ARG TESTS=yes

# Copy source code
COPY ./ /tau-lang

# # OR clone from git
# RUN git clone https://github.com/IDNI/tau-lang /tau-lang

WORKDIR /tau-lang

# if NIGHTLY is set to yes, then add .YYYY-MM-DD to the first line of the VERSION file
RUN if [ "$NIGHTLY" = "yes" ]; then echo "$(head -n 1 VERSION)-$(date --iso)" > VERSION; fi
RUN echo "Building nightly: $NIGHTLY" && echo "VERSION: $(cat VERSION)"

# Build tests and run them if TESTS is set to yes. Stop the build if they fail
RUN echo "Running tests: $TESTS"
RUN if [ "$TESTS" = "yes" ]; then ./release.sh -DTAU_BUILD_TESTS=ON && ./test-release.sh || exit 1; fi

# Linux packages
RUN ./packages.sh

# Windows packages
RUN cd external && ./libboost-mingw-builder.sh && cd ..
RUN ./w64-packages.sh
