cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# In these tests cases we check the results obtained in the normalized command.
# The output of the tests cases must be validated against expected results.

#include(test_repl-onf.cmake)
#include(test_repl-dnf.cmake)
include(test_repl-cnf.cmake)
#include(test_repl-nnf.cmake)
#include(test_repl-mnf.cmake)
#include(test_repl-snf.cmake)
