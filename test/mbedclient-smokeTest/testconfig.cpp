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

#include <stdio.h>
#include "mbed-client/m2mstring.h"
#include "testconfig.h"

TestConfig::TestConfig() { }

TestConfig::~TestConfig() {	}

void TestConfig::set_endpoint_name(const char *name) {
	_endpointName = m2m::String("lwm2mtest-");
	_endpointName += name;

	printf("MBED: endpoint=%s", _endpointName.c_str());
}

void TestConfig::setup() {
	char mds_in[32] = {0};
	char bs_in[32] = {0};
	char ep_name_in[16] = {0};

	printf("MBED: Waiting for test configuration from host...\r\n");
	scanf("%*2[<>]%31[^>]", bs_in);
	scanf("%*2[<>]%31[^>]", mds_in);
	scanf("%*2[<>]%15[^>]", ep_name_in);

	_bootstrapAddress = m2m::String(bs_in);
	_mdsAddress = m2m::String(mds_in);
	_endpointName = ep_name_in;
	_endpointType = m2m::String("test");

	_port = 5600;
	_lifetime = 2222;

	printf("MBED: test configuration \nbootstrap_server=%s\nmds_server=%s\nendpoint_name=%s\n", _bootstrapAddress.c_str(), _mdsAddress.c_str(), _endpointName.c_str());
}

m2m::String& TestConfig::get_bootstrap_server() {
	return _bootstrapAddress;
}

m2m::String& TestConfig::get_mds_server() {
	return _mdsAddress;
}

m2m::String& TestConfig::get_endpoint_name() {
	return _endpointName;
}

m2m::String& TestConfig::get_endpoint_type() {
	return _endpointType;
}

int TestConfig::get_port() {
	return _port++;
}

int TestConfig::get_lifetime() {
	return _lifetime;
}

