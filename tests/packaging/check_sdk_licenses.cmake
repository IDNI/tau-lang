# Installs the SDK box (the Library component) of BUILD_DIR into PREFIX and
# checks that it carries the license of everything it bundles.
#
#   cmake -DBUILD_DIR=<build> -DPREFIX=<scratch> -DWITH_CVC5=ON|OFF \
#         -P check_sdk_licenses.cmake

cmake_minimum_required(VERSION 3.22)

foreach(_var BUILD_DIR PREFIX WITH_CVC5)
	if(NOT DEFINED ${_var})
		message(FATAL_ERROR "check_sdk_licenses: ${_var} is not set")
	endif()
endforeach()

file(REMOVE_RECURSE "${PREFIX}")
execute_process(
	COMMAND "${CMAKE_COMMAND}" --install "${BUILD_DIR}"
		--component Library --prefix "${PREFIX}"
	RESULT_VARIABLE _rc
	OUTPUT_QUIET
	ERROR_VARIABLE _err)
if(NOT _rc EQUAL 0)
	message(FATAL_ERROR "check_sdk_licenses: the Library install failed:\n${_err}")
endif()

# The box is <libdir>/tau/sdk/<platform>; the libdir depends on the host.
file(GLOB _lic LIST_DIRECTORIES true "${PREFIX}/*/tau/sdk/*/licenses")
list(LENGTH _lic _n)
if(NOT _n EQUAL 1)
	file(REMOVE_RECURSE "${PREFIX}")
	message(FATAL_ERROR "check_sdk_licenses: expected one <libdir>/tau/sdk/<platform>/licenses, found ${_n}")
endif()
set(_expected
	LICENSE.md
	parser-sdk/LICENSE.md
	boost/LICENSE_1_0.txt
	unordered_dense/LICENSE)
if(WITH_CVC5)
	list(APPEND _expected
		cvc5/COPYING
		cadical/LICENSE
		symfpu/LICENSE
		gmp/SOURCE.md)
endif()

set(_missing "")
foreach(_f IN LISTS _expected)
	if(NOT EXISTS "${_lic}/${_f}")
		list(APPEND _missing "${_f}")
	endif()
endforeach()
# GMP's own license: the source COPYING files, or a distro copyright file.
if(WITH_CVC5)
	file(GLOB _gmp_licenses "${_lic}/gmp/*")
	list(FILTER _gmp_licenses EXCLUDE REGEX "/SOURCE\\.md$")
	if(NOT _gmp_licenses)
		list(APPEND _missing "gmp/<license>")
	endif()
endif()

file(REMOVE_RECURSE "${PREFIX}")
if(_missing)
	list(JOIN _missing ", " _missing)
	message(FATAL_ERROR "check_sdk_licenses: missing under ${_lic}: ${_missing}")
endif()
message(STATUS "check_sdk_licenses: ok")
