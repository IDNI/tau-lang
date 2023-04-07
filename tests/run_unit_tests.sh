#!/bin/bash

# This script executes tests.
# Run this script while in a build root directory (build-Debug/build-Release...)

status=0

#
# 
#
echo "Runing sample_test"
./sample_test
r=$?; [[ $r != 0 ]] && status=$r

#
# Running other test...
#
# echo "testing output"
# ./other_test >output.test.stdout 2>output.test.stderr
# r=$?; [[ $r != 0 ]] && status=$r
# O=`grep -v doctest output.test.stdout | head -n 1` && \
#	[ "$O" == "stdout test" ] \
#		|| (echo "ERROR: standard output fail" && status=1)
# O=`cat output.test.stderr` && \
#	[ "$O" == "stderr test" ] \
#		|| (echo "ERROR: standard error fail" && status=1)

#
# Cleaning
#
echo "Cleaning"
rm -f \
	output.test.stdout output.test.stderr 

#
# Returning status
#
exit $status
