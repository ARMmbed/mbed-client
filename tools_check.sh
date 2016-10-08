#!/bin/bash
# Copyright (c) 2016 ARM Limited. All rights reserved.
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

exists()
{
  command -v "$1" >/dev/null 2>&1
}

echo "Checking for tools needed by mbed Cloud Client development..."
missing=""
space=" "

tool="ninja"
if exists $tool ; then
	# TODO: check ninja version also!
	echo "Pre-requisites - "$tool" found - OK to continue."
else
	echo "ERROR - "$tool" is missing - it is a pre-requisite."
	echo "See - https://github.com/ninja-build/ninja"
	missing=$missing$space$tool"-build"
fi

tool="xsltproc"
if exists $tool ; then
	echo "Pre-requisites - "$tool" found - OK to continue."
else
	echo "ERROR - "$tool" is missing - it is a pre-requisite."
	missing=$missing$space$tool
fi

tool="lcov"
if exists $tool ; then
	echo "Pre-requisites - "$tool" found - OK to continue."
else
	echo "ERROR - "$tool" is missing - it is a pre-requisite."
	missing=$missing$space$tool
fi

tool="gcovr"
if exists $tool ; then
	echo "Pre-requisites - "$tool" found - OK to continue."
else
	echo "ERROR - "$tool" is missing - it is a pre-requisite."
	missing=$missing$space$tool
fi

tool="valgrind"
if exists $tool ; then
	echo "Pre-requisites - "$tool" found - OK to continue."
else
	echo "ERROR - "$tool" is missing - it is a pre-requisite."
	missing=$missing$space$tool
fi

tool="yotta"
if exists $tool ; then
	echo "Pre-requisites - "$tool" found - OK to continue."
else
	echo "ERROR - "$tool" is missing - it is a pre-requisite."
	missing=$missing$space$tool
fi

tool="mbed"
if exists $tool ; then
	echo "Pre-requisites - "$tool" found - OK to continue."
else
	echo "ERROR - "$tool" is missing - it is a pre-requisite."
	missing=$missing$space$tool
fi

tool="python"
if exists $tool ; then
	echo "Pre-requisites - "$tool" found - OK to continue."
else
	echo "ERROR - "$tool" is missing - it is a pre-requisite."
	missing=$missing$space$tool"-dev"
fi

tool="gcc"
if exists $tool ; then
	echo "Pre-requisites - "$tool" found - OK to continue."
else
	echo "ERROR - "$tool" is missing - it is a pre-requisite."
	missing=$missing$space$tool
fi

tool="git"
if exists $tool ; then
	echo "Pre-requisites - "$tool" found - OK to continue."
else
	echo "ERROR - "$tool" is missing - it is a pre-requisite."
	missing=$missing$space$tool"-dev"
fi

tool="gitk"
if exists $tool ; then
	echo "Pre-requisites - "$tool" found - OK to continue."
else
	echo "ERROR - "$tool" is missing - it is a pre-requisite."
	missing=$missing$space$tool"-dev"
fi

tool="cmake"
if exists $tool ; then
	echo "Pre-requisites - "$tool" found - OK to continue."
else
	echo "ERROR - "$tool" is missing - it is a pre-requisite."
	missing=$missing$space$tool
fi


# Check existense of cpputest by looking for the header file
if [[ ! -f /usr/include/CppUTest/MemoryLeakDetectorMallocMacros.h ]] ; then
    echo "ERROR - CppUTest missing - it is a pre-requisite."
    echo "Please read the accompanied README.md"
	missing = $missing" cpputest"
fi

if [ -n "$missing" ]; then 
	echo "NOT OK - tools mising."
	echo "run sudo apt-get install "$missing
	exit -1
fi
echo "Pre-requisites OK!"


