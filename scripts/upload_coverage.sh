#!/bin/bash
set -e

BUILD_DIR=`dirname "$BASH_SOURCE"`/../build
mkdir -p $BUILD_DIR/coverage
cd $BUILD_DIR/coverage
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_WITH_COVERAGE=ON ../..
cmake --build .

lcov -d . -z
ctest --verbose

lcov -d . -c -o "coverage.info"
lcov -r "coverage.info" "*/usr/*" "*/untests/*" -o "coverage.info"
lcov -l "coverage.info"

bash <(curl -s https://codecov.io/bash) -f "coverage.info" || echo "Codecov did not collect coverage reports"
