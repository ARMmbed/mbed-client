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
rm -rf results
rm -rf coverage
mkdir results
mkdir coverage

find ./ -name '*.xml' | xargs cp -t ./results/
find ./ -name '*.gcno' | xargs cp -t ./coverage/
find ./ -name '*.gcda' | xargs cp -t ./coverage/
exclude_files="${PWD}/test/"
gcovr --object-directory ./coverage --exclude '/usr' --exclude $exclude_files  -x -o ./results/gcovr.xml
rm -f ./coverage/*.gcno
rm -f ./coverage/*.gcda

echo
echo Create coverage document
echo
lcov -b ./ -d ./ -c -o ./coverage/app.info
lcov -q -r ./coverage/app.info "/test/mbedclient/*" -o ./coverage/app.info
lcov -q -r ./coverage/app.info "/usr*" -o ./coverage/app.info
genhtml -q --no-branch-coverage --function-coverage --demangle-cpp --title "mbed Client coverage" ./coverage/app.info -o ./coverage


