FROM ubuntu:24.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    bash cmake git g++ mingw-w64 libboost-all-dev nsis rpm

# Copy source code
COPY ./ /tau-lang

# # OR clone from git
# RUN git clone https://github.com/IDNI/tau-lang /tau-lang

WORKDIR /tau-lang

# Linux packages
RUN ./packages.sh

# Windows packages
RUN cd external && ./libboost-mingw-builder.sh && cd ..
RUN ./w64-packages.sh
