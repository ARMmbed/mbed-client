#!/bin/bash
# Copyright (c) 2015 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
# Licensed under the Apache License, Version 2.0 (the License); you may
# not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# * http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an AS IS BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

echo
echo "Build mbed Client API unit tests"
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
echo "Create coverage document"
echo
lcov -b ./ -d ./ -c -o ./coverage/app.info
lcov -q -r ./coverage/app.info "/test/mbedclient/*" -o ./coverage/app.info
lcov -q -r ./coverage/app.info "/usr*" -o ./coverage/app.info
genhtml -q --no-branch-coverage --function-coverage --demangle-cpp --title "mbed Client coverage" ./coverage/app.info -o ./coverage

echo 
echo "Creating report"
echo 
echo '<?xml version="1.0" encoding="UTF-8" ?>
<?xml-stylesheet type="text/xsl" href="junit_xsl.xslt"?>
<list>' >> index.xml

for f in results/*.xml
do
name=${f##*/}
echo '<entry name="results/'"$name"'" />'>> index.xml
done

echo '</list>' >> index.xml

echo
echo "Report created to index.xml (outputs html)"
echo
xsltproc -o results/testresults.html junit_xsl.xslt index.xml
rm -f index.xml

