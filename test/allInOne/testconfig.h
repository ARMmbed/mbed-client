/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */

#ifndef TESTCONFIG_H_
#define TESTCONFIG_H_

class TestConfig {

public:

	TestConfig();
	~TestConfig();

	void setup(const char *unique_name);
	void setup();

	void set_endpoint_name(const char *name);

	m2m::String& get_bootstrap_server();
	m2m::String& get_mds_server();
	m2m::String& get_endpoint_name();

private:

	m2m::String _endpointName;
	m2m::String _bootstrapAddress;
	m2m::String _mdsAddress;
	m2m::String _domain;

};

#endif /* TESTCONFIG_H_ */
