#!/bin/bash

# To view the license please visit https://github.com/IDNI/tau-lang/blob/main/LICENSE.txt

BUILD_OPTIONS=()

NAME="${1}"
if [ -z "${NAME}" ]; then
	echo "Error: name is required"
	exit 1
else
	case "${NAME}" in
		"python")
			BUILD_OPTIONS+=("-DTAU_BUILD_BINDING_PYTHON=ON")
			;;
		*)
			echo "Error: name can be only 'python'"
			exit 1
		;;
	esac
fi

ARGS_STARTS=2
BUILD_TYPE="${2}"
if [ -z "${BUILD_TYPE}" ]; then
	BUILD_TYPE="Release"
else
	case "${BUILD_TYPE}" in
		"Debug"|"Release"|"RelWithDebInfo")
			ARGS_STARTS=3
			;;
		*)
			echo "Error: build type must be either 'Debug', 'Release', or 'RelWithDebInfo'"
			exit 1
		;;
	esac
fi


./dev build "${BUILD_TYPE}" "${@:$ARGS_STARTS}" "${BUILD_OPTIONS[@]}"

