#!/bin/bash
echo
echo Build mbed Client API unit tests
echo
export RUN_UNIT_TESTS=1
yt target frdm-k64f-gcc
yt up
yt target x86-linux-native
yt up
yt clean
yt build
yt test --no-build -- -ojunit
unset RUN_UNIT_TESTS
echo
echo Create results
echo
mkdir results
find ./ -name '*.xml' | xargs cp -t ./results/

echo
echo Create coverage document
echo
rm -rf coverages
mkdir coverages
cd coverages

lcov -d ../build -c -o app.info
lcov -q -r app.info "/test*" -o app.info
lcov -q -r app.info "/usr*" -o app.info
genhtml -q --no-branch-coverage app.info
cd ..
echo
echo
echo
echo Have a nice bug hunt!
echo
echo
echo
