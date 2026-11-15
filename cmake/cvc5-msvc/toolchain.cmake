# The toolchain file of the cvc5 configure on the MSVC targets. It sets
# nothing: the compiler comes from CC and CXX. It exists so that the
# -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} that cvc5's FindGMP passes to
# its try_compile is not empty; CMake 4.5 fails to configure a try_compile
# project given an empty toolchain file, and cvc5 then stops at its GMP check.
