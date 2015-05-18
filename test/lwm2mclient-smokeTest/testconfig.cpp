/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */

#include <stdio.h>
#include "lwm2m-client/m2mstring.h"
#include "testconfig.h"

TestConfig::TestConfig() { }

TestConfig::~TestConfig() {	}

void TestConfig::set_endpoint_name(const char *name) {
	_endpointName = m2m::String("lwm2mtest-");
	_endpointName += name;

	printf("MBED: endpoint=%s", _endpointName.c_str());
}

void TestConfig::setup(const char *unique_name) {
	char mds_in[32] = {0};
	char bs_in[32] = {0};
	char domain_in[16] = {0};

	printf("MBED: Waiting for test configuration from host...\r\n");
	scanf("%*2[<>]%31[^>]", bs_in);
	scanf("%*2[<>]%31[^>]", mds_in);
	scanf("%*2[<>]%15[^>]", domain_in);

	_bootstrapAddress = m2m::String(bs_in);
	_mdsAddress = m2m::String(mds_in);
	_endpointName = m2m::String("lwm2mtest-");
	_endpointName += unique_name;
	_domain = m2m::String(domain_in);
	_endpointType = m2m::String("test");

	_port = 5600;
	_lifetime = 2222;

	printf("MBED: test configuration \nbootstrap_server=%s\nmds_server=%s\nendpoint_name=%s\ndomain=%s", _bootstrapAddress.c_str(), _mdsAddress.c_str(), _endpointName.c_str(), _domain.c_str());
}

void TestConfig::setup() {
	setup("\0");
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

