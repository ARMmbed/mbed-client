#!/bin/bash
# Execute script with root path where to find binaries. 
# For example ./run_unit_tests_with_valgrind.sh ./build/x86-linux-native-coverage/test/mbedclient/
#
# Copyright (c) 2015-2016 ARM Limited. All rights reserved.
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

if ./tools_check.sh
then
	echo "Tools-check OK"
else
	echo "Tools-check failure - STOP."
	exit 1
fi

input="binaries.txt"
valgrind_logs="valgrind_logs"
rm -rf $valgrind_logs
mkdir $valgrind_logs
find $1 -type f -executable -exec sh -c "file -i '{}' | grep -q 'x-executable; charset=binary'" \; -print > $input
while file= read -r binary
do
    valgrind --track-origins=yes --xml=yes --xml-file="${valgrind_logs}/valgrind_$(basename $binary).xml" "$binary"
done < "$input"
