#!/bin/bash
set -e

BUILD_DIR=`dirname "$BASH_SOURCE"`/../build
mkdir -p $BUILD_DIR/coverage
cd $BUILD_DIR/coverage
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_WITH_COVERAGE=ON ../..
cmake --build . -- -j8

lcov -d . -z
ctest --verbose

lcov -d . -c -o "coverage.info"
lcov -r "coverage.info" "*/usr/*" "*/catch.hpp" "*/catch_main.cpp" "*_tests.cpp" -o "coverage.info"
lcov -l "coverage.info"

bash <(curl -s https://codecov.io/bash) || echo "Codecov did not collect coverage reports"
