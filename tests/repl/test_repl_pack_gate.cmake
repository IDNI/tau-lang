# Checks tau_repl_unsupported against a pack of sbf and tau.
# Run as: cmake -P test_repl_pack_gate.cmake
# Script mode reads no project minimum, so without this line CMake 3.x parses
# IN_LIST as an unknown argument.
cmake_minimum_required(VERSION 3.22.1)
include(${CMAKE_CURRENT_LIST_DIR}/tau_repl_pack.cmake)

set(TAU_REGISTERED_BA_IDS sbf tau qint qlt bv hsb nlang)
set(TAU_PACK_BA_IDS sbf tau)

set(_failures 0)
function(expect cmd want)
	tau_repl_unsupported(_got "${cmd}")
	if(NOT "${_got}" STREQUAL "${want}")
		message(SEND_ERROR "tau_repl_unsupported(\"${cmd}\") = ${_got}, want ${want}")
	endif()
endfunction()

# A spec that names a missing algebra is skipped.
expect("i2:bv[8] := in console." TRUE)
expect("type byte = bv[8]" TRUE)
expect("realizable F o1[t]:qlt = {1/2}:qlt" TRUE)
expect("get qlt-t3-cap" TRUE)
expect("set nlang-http-timeout 3" TRUE)
expect("set nlang-model claude-opus-5-5" TRUE)
expect("i1:qint := in file(\"/src/tests/data.in\"). run 3 steps G (o1[t]:qint = i1[t]:qint)." TRUE)

# A spec of the pack's own algebras is kept.
expect("i1:sbf := in console. o1:sbf := out console." FALSE)
expect("sat x = 0" FALSE)

# Paths never count, wherever the checkout or its data live.
expect("i1:sbf := in file(\"/home/u/.worktrees/fix/data-game-bitblast-qlt/tests/sbf-ones.in\")." FALSE)
expect("i1:sbf := in file(\\\"/home/u/.worktrees/issue/bv-qlt/tests/x.in\\\"). o1:sbf := out console." FALSE)
expect("PATH=/opt/qlt/stubs:/usr/bin tau -e \"sat x = 0\"" FALSE)
expect("i1:sbf := in file(\"C:/work/hsb-nlang/qint/x.in\")." FALSE)
expect("i1:sbf := in file(\"${CMAKE_CURRENT_LIST_DIR}/../integration/test_files/bv-ones-length_10.in\")." FALSE)
