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

build_args=("${@:2}")
[[ ${#build_args[@]} -eq 0 ]] && build_args=("Release")
./dev build "${build_args[@]}" "${BUILD_OPTIONS[@]}"
