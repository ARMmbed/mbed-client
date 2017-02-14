/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
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
#include <string.h>
#include "CppUTest/TestHarness.h"
#include "test_uriqueryparser.h"

Test_UriQueryParser::Test_UriQueryParser()
{

}

Test_UriQueryParser::~Test_UriQueryParser()
{

}

void Test_UriQueryParser::test_query_string()
{
    char *address = "http://127.0.0.1:8080";
    CHECK(query_string(address) == NULL);

    char *address2 = "http://127.0.0.1:8080?";
    CHECK(query_string(address2) == NULL);

    char *address3 = "http://127.0.0.1:8080?param=test";
    STRCMP_EQUAL(query_string(address3), "param=test");

    char *address4 = "http://127.0.0.1:8080?param=test&test=abc";
    STRCMP_EQUAL(query_string(address4), "param=test&test=abc");
}

void Test_UriQueryParser::test_query_param_count()
{
    char *params = "a1&b2&c3";
    CHECK(query_param_count(params) == 0);

    char *params2 = "a=1&b=2&c=3";
    CHECK(query_param_count(params2) == 3);
}

void Test_UriQueryParser::test_uri_query_parameters()
{
    CHECK(uri_query_parameters(NULL, NULL) == false);
    CHECK(uri_query_parameters("", NULL) == false);

    char *static_query = "a=1&b=2&c=3";
    char *temp = (char*) malloc(strlen(static_query) + 1);
    memcpy(temp, static_query, strlen(static_query) + 1);
    char *output[3];

    CHECK(uri_query_parameters(temp, output) == true);
    STRCMP_EQUAL(output[0], "a=1");
    STRCMP_EQUAL(output[1], "b=2");
    STRCMP_EQUAL(output[2], "c=3");

    free(temp);
}
