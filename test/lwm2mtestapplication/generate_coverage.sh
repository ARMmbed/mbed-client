#!/bin/bash
rm -rf coverage
lcov --base-directory ../../ --directory ../../output/gcc_linux/source -c -o testapp.info
lcov --base-directory ../../ --directory ../../output/lwm2m-client-linux/source -c -o lwm2mlinux.info
lcov --base-directory ../../ --directory ../../output/lwm2m-client-mbedtls/source -c -o mbedtls.info
lcov --base-directory ../../../../nsdl-c --directory ../../../../nsdl-c/output/gcc_/source/libCoap/src -c -o libcoap.info
lcov --base-directory ../../../../nsdl-c --directory ../../../../nsdl-c/output/gcc_/source/libNsdl/src -c -o libnsdl.info
genhtml -o coverage -t "lwm2mtestapplication coverage" --num-spaces 4 testapp.info libcoap.info libnsdl.info lwm2mlinux.info mbedtls.info
lcov --base-directory ../../ --directory ../../output/gcc_linux/source -z
lcov --base-directory ../../ --directory ../../output/lwm2m-client-linux/source -z
lcov --base-directory ../../ --directory ../../output/lwm2m-client-mbedtls/source -z
lcov --base-directory ../../../../nsdl-c --directory ../../../../nsdl-c/output/gcc_/source/libCoap/src -z
lcov --base-directory ../../../../nsdl-c --directory ../../../../nsdl-c/output/gcc_/source/libNsdl/src -z