# `tau gen` cases: emit the artifact for one or more specs and never build it.
# A CMake script so the cases need no shell and no mktemp.
# Usage: cmake -DTAU=<tau> -DSPEC=<spec> [-DSPEC2=<spec2>] -DTMP=<scratch>
#               -DCASE=<one|two|two_with_output|cmake_names|stdin|stdin_twice>
#               -P this.
if(NOT TAU OR NOT SPEC OR NOT TMP OR NOT CASE)
	message(FATAL_ERROR "usage: -DTAU=<tau> -DSPEC=<spec> -DTMP=<scratch> "
		"-DCASE=<case> -P check_gen_verb.cmake")
endif()

file(REMOVE_RECURSE "${TMP}")
file(MAKE_DIRECTORY "${TMP}")
set(_s1 "${TMP}/s1.tau")
file(COPY_FILE "${SPEC}" "${_s1}")
set(_s2 "")
if(SPEC2)
	set(_s2 "${TMP}/s2.tau")
	file(COPY_FILE "${SPEC2}" "${_s2}")
endif()

if(CASE STREQUAL "two_with_output")
	# The gen branch never builds, so it exits 0; only `-o` with several
	# specs is a usage error, and that must not write anything.
	execute_process(COMMAND "${TAU}" gen "${_s1}" "${_s2}" -o "${TMP}/out"
		RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err
		TIMEOUT 300)
	if(_rc STREQUAL "0")
		message(FATAL_ERROR
			"tau gen -o with two specs succeeded\n${_out}${_err}")
	endif()
	if(EXISTS "${TMP}/out")
		message(FATAL_ERROR "-o with two specs wrote ${TMP}/out")
	endif()
elseif(CASE STREQUAL "stdin")
	# `tau gen -` reads the spec from stdin and, with no name, writes a.build/.
	execute_process(COMMAND "${TAU}" gen "-"
		INPUT_FILE "${SPEC}"
		WORKING_DIRECTORY "${TMP}"
		RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err
		TIMEOUT 300)
	if(NOT _rc STREQUAL "0")
		message(FATAL_ERROR "tau gen - exited ${_rc}\n${_out}${_err}")
	endif()
	foreach(_artifact "a.build/main.cpp" "a.build/CMakeLists.txt")
		if(NOT EXISTS "${TMP}/${_artifact}")
			message(FATAL_ERROR "no ${_artifact}\n${_out}${_err}")
		endif()
	endforeach()
elseif(CASE STREQUAL "stdin_twice")
	# stdin can be drained once; a second '-' is a usage error.
	execute_process(COMMAND "${TAU}" gen "-" "-"
		INPUT_FILE "${SPEC}"
		WORKING_DIRECTORY "${TMP}"
		RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err
		TIMEOUT 300)
	if(_rc STREQUAL "0")
		message(FATAL_ERROR "tau gen - - succeeded\n${_out}${_err}")
	endif()
elseif(CASE STREQUAL "reuse_dir")
	# A directory tau gen already marked is an artifact directory: a second
	# run into the same -o is accepted.
	file(MAKE_DIRECTORY "${TMP}/out")
	execute_process(COMMAND "${TAU}" gen "${_s1}" -o "${TMP}/out"
		RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err
		TIMEOUT 300)
	if(NOT _rc STREQUAL "0")
		message(FATAL_ERROR "tau gen -o <empty> exited ${_rc}\n${_out}${_err}")
	endif()
	execute_process(COMMAND "${TAU}" gen "${_s1}" -o "${TMP}/out"
		RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err
		TIMEOUT 300)
	if(NOT _rc STREQUAL "0")
		message(FATAL_ERROR "tau gen -o <artifact> exited ${_rc}\n${_out}${_err}")
	endif()
elseif(CASE STREQUAL "nonempty_dir")
	# -o into a non-empty directory that carries no artifact marker is
	# refused and leaves the directory alone.
	file(MAKE_DIRECTORY "${TMP}/out")
	file(WRITE "${TMP}/out/keep.txt" "keep\n")
	execute_process(COMMAND "${TAU}" gen "${_s1}" -o "${TMP}/out"
		RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err
		TIMEOUT 300)
	if(_rc STREQUAL "0")
		message(FATAL_ERROR "tau gen -o <nonempty> succeeded\n${_out}${_err}")
	endif()
	if(NOT EXISTS "${TMP}/out/keep.txt")
		message(FATAL_ERROR "tau gen -o <nonempty> removed keep.txt")
	endif()
	if(EXISTS "${TMP}/out/CMakeLists.txt")
		message(FATAL_ERROR "tau gen -o <nonempty> wrote CMakeLists.txt")
	endif()
else()
	# Every other case runs one spec (and a second for `two`).
	set(_gen_args "${_s1}")
	if(CASE STREQUAL "two")
		list(APPEND _gen_args "${_s2}")
	endif()
	execute_process(COMMAND "${TAU}" gen ${_gen_args}
		RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err
		TIMEOUT 300)
	if(NOT _rc STREQUAL "0")
		message(FATAL_ERROR "tau gen exited ${_rc}\n${_out}${_err}")
	endif()
	foreach(_s "${_s1}" ${_s2})
		foreach(_artifact "${_s}.build/main.cpp"
				"${_s}.build/CMakeLists.txt")
			if(NOT EXISTS "${_artifact}")
				message(FATAL_ERROR "no ${_artifact}\n${_out}${_err}")
			endif()
		endforeach()
	endforeach()
	if(CASE STREQUAL "cmake_names")
		file(READ "${_s1}.build/CMakeLists.txt" _cmake)
		if(NOT _cmake MATCHES "find_package\\(Tau")
			message(FATAL_ERROR "the emitted CMakeLists does not "
				"find_package(Tau:\n${_cmake}")
		endif()
	endif()
endif()

file(REMOVE_RECURSE "${TMP}")
