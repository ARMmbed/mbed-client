/*
 * Copyright (c) 2016 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "CppUTest/TestHarness.h"
#include "test_m2mstringbufferbase.h"
//#include "m2mstringbuffer.h"



Test_M2Mstringbufferbase::Test_M2Mstringbufferbase()
{
}

Test_M2Mstringbufferbase::~Test_M2Mstringbufferbase()
{
}

void Test_M2Mstringbufferbase::test_ensure_space()
{
    CHECK(ensure_space(1, 1));
    CHECK(!ensure_space(1, 2));
}

void Test_M2Mstringbufferbase::test_append()
{
    char data[2];

    _curr_size = 0;
    CHECK(append(data, 2, 'a'));
    CHECK(!append(data, 2, 'a'));

    _curr_size = 0;
    CHECK(append(data, 2, "a"));
    CHECK(!append(data, 2, "a"));

    _curr_size = 0;
    CHECK(!append(data, 2, "aa"));

    _curr_size = 0;
    CHECK(append(data, 2, "aa", 1));

    _curr_size = 0;
    CHECK(!append(data, 2, "aa", 2));

    
}

void Test_M2Mstringbufferbase::test_append_int()
{
    char data[2];
    _curr_size = 0;
    CHECK(append_int(data, 2, 1));
    CHECK(!append_int(data, 2, 2));
    _curr_size = 0;
    CHECK(!append_int(data, 2, 666));
}

void Test_M2Mstringbufferbase::test_find_last_of()
{
    char data[2];
    _curr_size = 0;
    CHECK(append(data, 2, 'a'));
    CHECK(0 == find_last_of(data, 'a'));
    CHECK(0 > find_last_of(data, 'b'));
}

/*
    bool ensure_space(size_t max_size, size_t required_size) const;

    bool append(char *buff, size_t max_size, char data);

    bool append(char *buff, size_t max_size, const char *data);

    bool append(char *buff, size_t max_size, const char *data, size_t data_len);

    bool append_int(char *buff, size_t max_size, uint16_t data);

    int find_last_of(const char *buff, char search_char) const;*/
