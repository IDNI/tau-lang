cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# Resolve a NATIVE tgf executable for parser generation.
#
# generate_parser() (generate-parser.cmake) needs a tgf binary it can run at
# build time. Every target consumes the host tgf store package, which is keyed
# by the host triple and built with native clang, so a cross toolchain (w64,
# wasm) never produces a tgf the build host cannot run. This module sets
# TAU_TGF_EXECUTABLE (the binary to run) and TAU_TGF_DEPEND (what to depend on
# to have it built).
set(TAU_TGF_EXECUTABLE "${TAU_TGF_PACKAGE_PREFIX}/bin/tgf")
# A native MSVC package installs tgf.exe; a host-clang package for a MinGW
# cross build installs a bare tgf even though WIN32 is set here.
if(WIN32 AND EXISTS "${TAU_TGF_PACKAGE_PREFIX}/bin/tgf.exe")
	set(TAU_TGF_EXECUTABLE "${TAU_TGF_PACKAGE_PREFIX}/bin/tgf.exe")
endif()
set(TAU_TGF_DEPEND "")
