/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "CppUTest/TestHarness.h"
#include "test_m2mdevice.h"
#include "m2mobject_stub.h"
#include "m2mobjectinstance_stub.h"
#include "m2mresource_stub.h"
#include "m2mbase_stub.h"

Test_M2MDevice::Test_M2MDevice()
{
    m2mobject_stub::inst = new M2MObjectInstance("name");
    //m2mobjectinstance_stub::resource = new M2MResource("name","type",M2MBase::Dynamic);
    m2mresource_stub::bool_value = true;
//    m2mobjectinstance_stub::create_resource = new M2MResource("name","type",M2MBase::Dynamic);
    device = M2MDevice::get_instance();

    delete m2mobjectinstance_stub::create_resource;
    m2mobjectinstance_stub::create_resource = NULL;
}

Test_M2MDevice::~Test_M2MDevice()
{
    delete m2mobject_stub::inst;
    m2mobject_stub::inst = NULL;

    M2MDevice::delete_instance();
    device = NULL;
}

void Test_M2MDevice::test_create_resource_string()
{
    CHECK(device->create_resource(M2MDevice::Reboot,"test") == NULL);
    CHECK(device->create_resource(M2MDevice::ErrorCode,"test") == NULL);
    CHECK(device->create_resource(M2MDevice::SupportedBindingMode,"test") == NULL);

//    m2mobjectinstance_stub::create_resource = new M2MResource("name","type",M2MBase::Dynamic);

    CHECK(device->create_resource(M2MDevice::Manufacturer,"test") != NULL);
    CHECK(M2MBase::GET_PUT_ALLOWED == m2mbase_stub::operation);

//    m2mobjectinstance_stub::resource = new M2MResource("name","type",M2MBase::Dynamic);

    //Duplicate resource
    CHECK(device->create_resource(M2MDevice::Manufacturer,"test") == NULL);

    delete m2mobjectinstance_stub::resource;
    m2mobjectinstance_stub::resource = NULL;

    CHECK(device->create_resource(M2MDevice::DeviceType,"test") != NULL);
    CHECK(M2MBase::GET_PUT_ALLOWED == m2mbase_stub::operation);


    CHECK(device->create_resource(M2MDevice::ModelNumber,"test") != NULL);
    CHECK(M2MBase::GET_PUT_ALLOWED == m2mbase_stub::operation);


    CHECK(device->create_resource(M2MDevice::SerialNumber,"test") != NULL);
    CHECK(M2MBase::GET_PUT_ALLOWED == m2mbase_stub::operation);


    CHECK(device->create_resource(M2MDevice::HardwareVersion,"test") != NULL);
    CHECK(M2MBase::GET_PUT_ALLOWED == m2mbase_stub::operation);


    CHECK(device->create_resource(M2MDevice::FirmwareVersion,"test") != NULL);
    CHECK(M2MBase::GET_PUT_ALLOWED == m2mbase_stub::operation);


    CHECK(device->create_resource(M2MDevice::SoftwareVersion,"test") != NULL);
    CHECK(M2MBase::GET_PUT_ALLOWED == m2mbase_stub::operation);


    CHECK(device->create_resource(M2MDevice::UTCOffset,"test") != NULL);
    CHECK(M2MBase::GET_PUT_POST_ALLOWED == m2mbase_stub::operation);


    CHECK(device->create_resource(M2MDevice::Timezone,"test") != NULL);
    CHECK(M2MBase::GET_PUT_POST_ALLOWED == m2mbase_stub::operation);

    CHECK(device->create_resource(M2MDevice::BatteryLevel,"test") == NULL);
    CHECK(device->create_resource(M2MDevice::BatteryStatus,"test") == NULL);
    CHECK(device->create_resource(M2MDevice::MemoryFree,"test") == NULL);
    CHECK(device->create_resource(M2MDevice::MemoryTotal,"test") == NULL);
    CHECK(device->create_resource(M2MDevice::ResetErrorCode,"test") == NULL);
    CHECK(device->create_resource(M2MDevice::CurrentTime,"test") == NULL);
    CHECK(device->create_resource(M2MDevice::AvailablePowerSources,"test") == NULL);
    CHECK(device->create_resource(M2MDevice::PowerSourceVoltage,"test") == NULL);
    CHECK(device->create_resource(M2MDevice::PowerSourceCurrent,"test") == NULL);
    CHECK(device->create_resource(M2MDevice::FactoryReset,"test") == NULL);

    delete m2mobjectinstance_stub::create_resource;
    m2mobjectinstance_stub::create_resource = NULL;
}

void Test_M2MDevice::test_create_resource_int()
{
//    m2mobjectinstance_stub::create_resource = new M2MResource("name","type",M2MBase::Dynamic);

    CHECK(device->create_resource(M2MDevice::Reboot,10) == NULL);
    CHECK(device->create_resource(M2MDevice::SupportedBindingMode,10) == NULL);

    CHECK(device->create_resource(M2MDevice::Manufacturer,10) == NULL);
    CHECK(device->create_resource(M2MDevice::DeviceType,10) == NULL);
    CHECK(device->create_resource(M2MDevice::ModelNumber,10) == NULL);
    CHECK(device->create_resource(M2MDevice::SerialNumber,10) == NULL);
    CHECK(device->create_resource(M2MDevice::HardwareVersion,10) == NULL);
    CHECK(device->create_resource(M2MDevice::FirmwareVersion,10) == NULL);
    CHECK(device->create_resource(M2MDevice::SoftwareVersion,10) == NULL);
    CHECK(device->create_resource(M2MDevice::UTCOffset,10) == NULL);
    CHECK(device->create_resource(M2MDevice::Timezone,10) == NULL);
    CHECK(device->create_resource(M2MDevice::FactoryReset,10) == NULL);

    CHECK(device->create_resource(M2MDevice::ErrorCode,10) != NULL);

    CHECK(device->create_resource(M2MDevice::BatteryLevel,10) != NULL);
    CHECK(M2MBase::GET_PUT_ALLOWED == m2mbase_stub::operation);

    CHECK(device->create_resource(M2MDevice::BatteryStatus,10) != NULL);
    CHECK(M2MBase::GET_PUT_ALLOWED == m2mbase_stub::operation);

    CHECK(device->create_resource(M2MDevice::MemoryFree,10) != NULL);
    CHECK(M2MBase::GET_PUT_ALLOWED == m2mbase_stub::operation);

    CHECK(device->create_resource(M2MDevice::MemoryTotal,10) != NULL);
    CHECK(M2MBase::GET_PUT_ALLOWED == m2mbase_stub::operation);

    CHECK(device->create_resource(M2MDevice::CurrentTime,10) != NULL);
    CHECK(M2MBase::GET_PUT_POST_ALLOWED == m2mbase_stub::operation);

    CHECK(device->create_resource(M2MDevice::AvailablePowerSources,10) != NULL);
    CHECK(M2MBase::GET_PUT_ALLOWED == m2mbase_stub::operation);

    CHECK(device->create_resource(M2MDevice::PowerSourceVoltage,10) != NULL);
    CHECK(M2MBase::GET_PUT_ALLOWED == m2mbase_stub::operation);

    CHECK(device->create_resource(M2MDevice::PowerSourceCurrent,10) != NULL);
    CHECK(M2MBase::GET_PUT_ALLOWED == m2mbase_stub::operation);

    delete m2mobjectinstance_stub::create_resource;
    m2mobjectinstance_stub::create_resource = NULL;
}

void Test_M2MDevice::test_create_resource_no_param()
{
//    m2mobjectinstance_stub::create_resource = new M2MResource("name","type",M2MBase::Dynamic);

    CHECK(device->create_resource(M2MDevice::Reboot) == NULL);
    CHECK(device->create_resource(M2MDevice::ErrorCode) == NULL);
    CHECK(device->create_resource(M2MDevice::SupportedBindingMode) == NULL);

    CHECK(device->create_resource(M2MDevice::Manufacturer) == NULL);
    CHECK(device->create_resource(M2MDevice::DeviceType) == NULL);
    CHECK(device->create_resource(M2MDevice::ModelNumber) == NULL);
    CHECK(device->create_resource(M2MDevice::SerialNumber) == NULL);
    CHECK(device->create_resource(M2MDevice::HardwareVersion) == NULL);
    CHECK(device->create_resource(M2MDevice::FirmwareVersion) == NULL);
    CHECK(device->create_resource(M2MDevice::SoftwareVersion) == NULL);
    CHECK(device->create_resource(M2MDevice::UTCOffset) == NULL);
    CHECK(device->create_resource(M2MDevice::Timezone) == NULL);

    CHECK(device->create_resource(M2MDevice::BatteryLevel) == NULL);
    CHECK(device->create_resource(M2MDevice::BatteryStatus) == NULL);
    CHECK(device->create_resource(M2MDevice::MemoryFree) == NULL);
    CHECK(device->create_resource(M2MDevice::MemoryTotal) == NULL);
    CHECK(device->create_resource(M2MDevice::CurrentTime) == NULL);
    CHECK(device->create_resource(M2MDevice::AvailablePowerSources) == NULL);
    CHECK(device->create_resource(M2MDevice::PowerSourceVoltage) == NULL);
    CHECK(device->create_resource(M2MDevice::PowerSourceCurrent) == NULL);

    CHECK(device->create_resource(M2MDevice::ResetErrorCode) != NULL);

    CHECK(device->create_resource(M2MDevice::FactoryReset) != NULL);
    CHECK(M2MBase::POST_ALLOWED == m2mbase_stub::operation);

    delete m2mobjectinstance_stub::create_resource;
    m2mobjectinstance_stub::create_resource = NULL;
}

void Test_M2MDevice::test_delete_resource()
{
    m2mobjectinstance_stub::bool_value = true;

    CHECK(device->delete_resource(M2MDevice::Reboot) == false);
    CHECK(device->delete_resource(M2MDevice::ErrorCode) == false);
    CHECK(device->delete_resource(M2MDevice::SupportedBindingMode) == false);

    CHECK(device->delete_resource(M2MDevice::Manufacturer) == true);
    CHECK(device->delete_resource(M2MDevice::BatteryLevel) == true);
    CHECK(device->delete_resource(M2MDevice::FactoryReset) == true);
}

void Test_M2MDevice::test_set_resource_value_string()
{
    m2mbase_stub::bool_value = true;

//    m2mobjectinstance_stub::resource = new M2MResource("name","type",M2MBase::Dynamic);

    CHECK(device->set_resource_value(M2MDevice::Reboot,"test") == false);
    CHECK(device->set_resource_value(M2MDevice::ErrorCode,"test") == false);
    CHECK(device->set_resource_value(M2MDevice::SupportedBindingMode,"test") == false);

    CHECK(device->set_resource_value(M2MDevice::Manufacturer,"test") == true);
    CHECK(device->set_resource_value(M2MDevice::DeviceType,"test") == true);
    CHECK(device->set_resource_value(M2MDevice::ModelNumber,"test") == true);
    CHECK(device->set_resource_value(M2MDevice::SerialNumber,"test") == true);
    CHECK(device->set_resource_value(M2MDevice::HardwareVersion,"test") == true);
    CHECK(device->set_resource_value(M2MDevice::FirmwareVersion,"test") == true);
    CHECK(device->set_resource_value(M2MDevice::SoftwareVersion,"test") == true);
    CHECK(device->set_resource_value(M2MDevice::UTCOffset,"test") == true);
    CHECK(device->set_resource_value(M2MDevice::Timezone,"test") == true);

    CHECK(device->set_resource_value(M2MDevice::BatteryLevel,"test") == false);
    CHECK(device->set_resource_value(M2MDevice::BatteryStatus,"test") == false);
    CHECK(device->set_resource_value(M2MDevice::MemoryFree,"test") == false);
    CHECK(device->set_resource_value(M2MDevice::MemoryTotal,"test") == false);
    CHECK(device->set_resource_value(M2MDevice::ResetErrorCode,"test") == false);
    CHECK(device->set_resource_value(M2MDevice::CurrentTime,"test") == false);
    CHECK(device->set_resource_value(M2MDevice::AvailablePowerSources,"test") == false);
    CHECK(device->set_resource_value(M2MDevice::PowerSourceVoltage,"test") == false);
    CHECK(device->set_resource_value(M2MDevice::PowerSourceCurrent,"test") == false);
    CHECK(device->set_resource_value(M2MDevice::FactoryReset,"test") == false);

    delete m2mobjectinstance_stub::resource;
    m2mobjectinstance_stub::resource = NULL;
}

void Test_M2MDevice::test_set_resource_value_int()
{
    m2mbase_stub::bool_value = true;

//    m2mobjectinstance_stub::resource = new M2MResource("name","type",M2MBase::Dynamic);

    CHECK(device->set_resource_value(M2MDevice::Reboot,10) == false);
    CHECK(device->set_resource_value(M2MDevice::ErrorCode,10) == true);
    CHECK(device->set_resource_value(M2MDevice::SupportedBindingMode,10) == false);

    CHECK(device->set_resource_value(M2MDevice::Manufacturer,10) == false);
    CHECK(device->set_resource_value(M2MDevice::DeviceType,10) == false);
    CHECK(device->set_resource_value(M2MDevice::ModelNumber,10) == false);
    CHECK(device->set_resource_value(M2MDevice::SerialNumber,10) == false);
    CHECK(device->set_resource_value(M2MDevice::HardwareVersion,10) == false);
    CHECK(device->set_resource_value(M2MDevice::FirmwareVersion,10) == false);
    CHECK(device->set_resource_value(M2MDevice::SoftwareVersion,10) == false);
    CHECK(device->set_resource_value(M2MDevice::UTCOffset,10) == false);
    CHECK(device->set_resource_value(M2MDevice::Timezone,10) == false);

    CHECK(device->set_resource_value(M2MDevice::BatteryLevel,10) == true);
    CHECK(device->set_resource_value(M2MDevice::BatteryStatus,10) == true);
    CHECK(device->set_resource_value(M2MDevice::MemoryFree,10) == true);
    CHECK(device->set_resource_value(M2MDevice::MemoryTotal,10) == true);
    CHECK(device->set_resource_value(M2MDevice::ErrorCode,10) == true);
    CHECK(device->set_resource_value(M2MDevice::CurrentTime,10) == true);
    CHECK(device->set_resource_value(M2MDevice::AvailablePowerSources,10) == true);
    CHECK(device->set_resource_value(M2MDevice::PowerSourceVoltage,10) == true);
    CHECK(device->set_resource_value(M2MDevice::PowerSourceCurrent,10) == true);
    CHECK(device->set_resource_value(M2MDevice::FactoryReset,10) == false);

    delete m2mobjectinstance_stub::resource;
    m2mobjectinstance_stub::resource = NULL;
}

void Test_M2MDevice::test_resource_value_int()
{
    uint8_t value[] = {"10"};
    uint8_t* ptr = (uint8_t*)malloc((uint32_t)sizeof(value));
    m2mbase_stub::value = ptr;
    memset(m2mbase_stub::value,0,(uint32_t)sizeof(value));
    memcpy(m2mbase_stub::value,value,sizeof(value));
    m2mbase_stub::int_value = (uint32_t)sizeof(value);

//    m2mobjectinstance_stub::resource = new M2MResource("name","type",M2MBase::Dynamic);

    CHECK(device->resource_value_int(M2MDevice::BatteryLevel) == 10);
    CHECK(device->resource_value_int(M2MDevice::BatteryStatus) == 10);
    CHECK(device->resource_value_int(M2MDevice::MemoryFree) == 10);
    CHECK(device->resource_value_int(M2MDevice::MemoryTotal) == 10);
    CHECK(device->resource_value_int(M2MDevice::ErrorCode) == 10);
    CHECK(device->resource_value_int(M2MDevice::CurrentTime) == 10);
    CHECK(device->resource_value_int(M2MDevice::AvailablePowerSources) == 10);
    CHECK(device->resource_value_int(M2MDevice::PowerSourceVoltage) == 10);
    CHECK(device->resource_value_int(M2MDevice::PowerSourceCurrent) == 10);
    CHECK(device->resource_value_int(M2MDevice::Manufacturer) == -1);

    delete m2mobjectinstance_stub::resource;
    m2mobjectinstance_stub::resource = NULL;

    free(ptr);
}

void Test_M2MDevice::test_resource_value_string()
{
    String test = "string";
    uint8_t value[] = {"string"};
    m2mbase_stub::value = (uint8_t*)malloc((uint32_t)sizeof(value));
    memset(m2mbase_stub::value,0,(uint32_t)sizeof(value));
    memcpy(m2mbase_stub::value,value,sizeof(value));
    m2mbase_stub::int_value = (uint32_t)sizeof(value);

//    m2mobjectinstance_stub::resource = new M2MResource("name","type",M2MBase::Dynamic);

    CHECK(device->resource_value_string(M2MDevice::Manufacturer) == test);
    CHECK(device->resource_value_string(M2MDevice::DeviceType) == test);
    CHECK(device->resource_value_string(M2MDevice::ModelNumber) == test);
    CHECK(device->resource_value_string(M2MDevice::SerialNumber) == test);
    CHECK(device->resource_value_string(M2MDevice::HardwareVersion) == test);
    CHECK(device->resource_value_string(M2MDevice::FirmwareVersion) == test);
    CHECK(device->resource_value_string(M2MDevice::SoftwareVersion) == test);
    CHECK(device->resource_value_string(M2MDevice::UTCOffset) == test);
    CHECK(device->resource_value_string(M2MDevice::Timezone) == test);
    CHECK(device->resource_value_string(M2MDevice::BatteryLevel) == "");

    delete m2mobjectinstance_stub::resource;
    m2mobjectinstance_stub::resource = NULL;

    free(m2mbase_stub::value);
    m2mbase_stub::value = NULL;
}

void Test_M2MDevice::test_is_resource_present()
{
//    m2mobjectinstance_stub::resource = new M2MResource("name","type",M2MBase::Dynamic);
    CHECK(device->is_resource_present(M2MDevice::Reboot) == true);

    delete m2mobjectinstance_stub::resource;
    m2mobjectinstance_stub::resource = NULL;

    CHECK(device->is_resource_present(M2MDevice::Manufacturer) == false);
    CHECK(device->is_resource_present(M2MDevice::DeviceType) == false);
}

void Test_M2MDevice::test_per_resource_count()
{
    m2mobjectinstance_stub::int_value = 1;
    CHECK(device->per_resource_count(M2MDevice::Reboot) == 1);

    m2mobjectinstance_stub::int_value = 0;

    CHECK(device->per_resource_count(M2MDevice::Manufacturer) == 0);
}

void Test_M2MDevice::test_total_resource_count()
{
//    M2MResource res("test","test",M2MBase::Dynamic);

//    m2mobjectinstance_stub::resource_list.push_back(&res);
//    M2MResource res2("test","test",M2MBase::Dynamic);

//    m2mobjectinstance_stub::resource_list.push_back(&res2);

//    CHECK(device->total_resource_count() == 2);

//    m2mobjectinstance_stub::resource_list.clear();
}
