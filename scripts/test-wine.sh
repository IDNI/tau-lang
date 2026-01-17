#!/bin/bash

BUILD_TYPE=${1:-Release}
TESTS_DIR=../build-$BUILD_TYPE
STATUS=0

echo "Building $BUILD_TYPE exe files"
./dev build $BUILD_TYPE -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/mingw-w64-x86_64.cmake -DTAU_BUILD_TESTS=ON

cd tests
for TEST in $TESTS_DIR/test_*.exe; do
	echo "Running wine $TEST"
	wine "$TEST"
	S=$?
	if [ $S -ne 0 ]; then
		STATUS=$S
		echo "Test $TEST failed with status $STATUS"
	fi
done

if [ $STATUS -ne 0 ]; then
	echo "One or more tests failed"
	exit $STATUS
fi

echo "All tests passed"