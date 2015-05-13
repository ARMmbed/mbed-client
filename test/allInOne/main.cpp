/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "mbed-net-sockets/UDPSocket.h"
#include "EthernetInterface.h"
#include "test_env.h"
#include "lwm2m-client/m2minterfacefactory.h"
#include "lwm2m-client/m2minterfaceobserver.h"
#include "lwm2m-client/m2mdevice.h"
#include "lwm2m-client/m2mobjectinstance.h"
#include "lwm2m-client/m2minterface.h"
#include "testconfig.h"

// TODO: Remove when yotta supports init.
#include "lwipv4_init.h"

const String &MANUFACTURER = "ARM";
const String &TYPE = "type";

// Dynamic resource variables
const String &DYNAMIC_RESOURCE_NAME = "Dynamic";
const String &DYNAMIC_RESOURCE_TYPE = "DynamicType";
const String &STATIC_RESOURCE_NAME = "Static";
const String &STATIC_RESOURCE_TYPE = "StaticType";
const uint8_t STATIC_VALUE[] = "Static value";

class M2MLWClient: public M2MInterfaceObserver {
public:
    M2MLWClient(TestConfig *test_config){
        _interface = NULL;
        _register_security = NULL;
        _resource_object = NULL;
        _bootstrapped = false;
        _error = false;
        _registered = false;
        _unregistered = false;
        _registration_updated = false;
        _resource_value = 0;
        _object = NULL;
        _test_config = test_config;
    }

    virtual ~M2MLWClient() {
        if(_interface) {
            delete _interface;
        }
        if( _register_security){
            delete _register_security;
        }
    }

    bool create_interface() {
    	bool success = false;
         // Creates M2MInterface using which endpoint can
         // setup its name, resource type, life time, connection mode,
         // Currently only LwIPv4 is supported.
         _interface = M2MInterfaceFactory::create_interface( *this,
												   _test_config->get_endpoint_name(),
												   _test_config->get_endpoint_type(),
												   _test_config->get_lifetime(),
												   _test_config->get_port(),
                                                   "",
                                                   M2MInterface::UDP,
                                                   M2MInterface::LwIP_IPv4,
                                                   "");
         if (_interface) {
        	 success = true;
         }

         return success;
    }

    bool bootstrap_successful() {
        return _bootstrapped;
    }

    M2MSecurity* create_bootstrap_object() {
        // Creates bootstrap server object with Bootstrap server address and other parameters
        // required for client to connect to bootstrap server.
        M2MSecurity *security = M2MInterfaceFactory::create_security(M2MSecurity::Bootstrap);
        if(security) {
            security->set_resource_value(M2MSecurity::M2MServerUri, _test_config->get_bootstrap_server());
            security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::NoSecurity);
        }
        return security;
    }

    void test_bootstrap(M2MSecurity *security) {
        if(_interface) {
             // Bootstrap function.
            _interface->bootstrap(security);
        }
    }

    bool register_successful() {
        return _registered;
    }

    bool unregister_successful() {
        return _unregistered;
    }

    bool update_register_successful() {
        return _registration_updated;
    }

    M2MSecurity* create_register_object() {
        // Creates server object with LWM2M server address and other parameters
        // required for client to connect to LWM2M server.
        M2MSecurity *security = M2MInterfaceFactory::create_security(M2MSecurity::M2MServer);
        if(security) {
            security->set_resource_value(M2MSecurity::M2MServerUri, _test_config->get_mds_server());
            security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::NoSecurity);
        }
        return security;
    }

    void test_register(M2MObjectList object_list){
        if(_interface) {
            // Register function
            _interface->register_object(_register_security, object_list);
        }
    }

    bool test_update_register(const uint32_t lifetime)
    {
        bool success = false;
        if(_interface && _register_security) {
            success = true;
            _interface->update_registration(_register_security,lifetime);
        }
        return success;
    }

    void test_unregister(){
    	if(_interface) {
    		// Unregister function
    		_interface->unregister_object(NULL);
    	}
    }

    void set_register_object(M2MSecurity *&register_object){
        if(_register_security) {
            delete _register_security;
            _register_security = NULL;
        }
        _register_security = register_object;
    }

    M2MDevice* create_device_object() {
    	M2MDevice *device = M2MInterfaceFactory::create_device();
    	if (device) {
    		device->create_resource(M2MDevice::Manufacturer, MANUFACTURER);
    		device->create_resource(M2MDevice::DeviceType, TYPE);
    	}
    	return device;
    }

    M2MObject* create_generic_object() {
        _object = M2MInterfaceFactory::create_object("Test");
        if(_object) {
            M2MObjectInstance* inst = _object->create_object_instance();
            if(inst) {
                    M2MResource* res = inst->create_dynamic_resource("D","ResourceTest",true);
                    char buffer[20];
                    int size = sprintf(buffer,"%d",_resource_value);
                    res->set_operation(M2MBase::GET_PUT_POST_ALLOWED);
                    res->set_value((const uint8_t*)buffer,
                                   (const uint32_t)size,
                                   true);
                    _resource_value++;

                    inst->create_static_resource("S",
                                                 "ResourceTest",
                                                 STATIC_VALUE,
                                                 sizeof(STATIC_VALUE)-1);
            }
        }
        return _object;
    }

    //Callback from mbed client stack when the bootstrap
    // is successful, it returns the mbed Device Server object
    // which will be used for registering the resources to
    // mbed Device server.
    void bootstrap_done(M2MSecurity *server_object){
    	if(server_object) {
            _bootstrapped = true;
            _error = false;
        }
    }

    //Callback from mbed client stack when the registration
    // is successful, it returns the mbed Device Server object
    // to which the resources are registered and registered objects.
    void object_registered(M2MSecurity */*security_object*/, const M2MServer &/*server_object*/){
        _registered = true;
        _unregistered = false;
    }

    //Callback from mbed client stack when the registration update
        // is successful, it returns the mbed Device Server object
        // to which the resources are registered and registered objects.
    void registration_updated(M2MSecurity */*security_object*/, const M2MServer &/*server_object*/){
            _registration_updated = true;
        }

    //Callback from mbed client stack when the unregistration
    // is successful, it returns the mbed Device Server object
    // to which the resources were unregistered.
    void object_unregistered(M2MSecurity */*server_object*/){
        _unregistered = true;
        _registered = false;
    }

    //Callback from mbed client stack if any value has changed
    // during PUT operation. Object and its type is passed in
    // the callback.
    void value_updated(M2MBase *base, M2MBase::BaseType type) {
        printf("\nValue updated of Object name %s and Type %d\n",
               base->name().c_str(), type);
    }

    //Callback from mbed client stack if any error is encountered
    // during any of the LWM2M operations. Error type is passed in
    // the callback.
    void error(M2MInterface::Error error){
        _error = true;
        printf("\nError occured %d\n", error);
    }

private:

    M2MInterface    	*_interface;
    M2MSecurity         *_register_security;
    M2MObject			*_resource_object;
    M2MObject           *_object;
    volatile bool       _bootstrapped;
    volatile bool       _error;
    volatile bool       _registered;
    volatile bool       _unregistered;
    volatile bool       _registration_updated;
    int                 _resource_value;
    TestConfig			*_test_config;
};

#define SUITE_TEST_INFO(test_name, info)		printf("Suite-%s: %s\n", test_name, info)
#define SUITE_TEST_RESULT(test_name, result)	printf("Suite-%s: result %s\n", test_name, result ? "pass" : "fail")
#define SUITE_RESULT(result)					printf("Suite: result %s\n", result ? "success" : "failure")

bool test_bootstrap(TestConfig *test_config) {
	SUITE_TEST_INFO("test_bootstrap", "started");

	bool interface_success = false;
	bool bootstrap_success = false;

    // Instantiate the class which implements
    // LWM2M Client API
    M2MLWClient *lwm2mclient = new M2MLWClient(test_config);

    SUITE_TEST_INFO("test_bootstrap", "client done");

    // Create LWM2M Client API interface to manage bootstrap,
    // register and unregister
    interface_success = lwm2mclient->create_interface();

    // Create LWM2M bootstrap object specifying bootstrap server
    // information.
    M2MSecurity* security_object = lwm2mclient->create_bootstrap_object();

    // Issue bootstrap command.
    lwm2mclient->test_bootstrap(security_object);

    SUITE_TEST_INFO("test_bootstrap", "bootstrap done");
    // Wait till the bootstrap callback is called successfully.
    // Callback comes in bootstrap_done()
    while (!(bootstrap_success = lwm2mclient->bootstrap_successful())) { __WFI(); }

    // Delete security object created for bootstrapping
    if(security_object) {
        delete security_object;
    }

    if (lwm2mclient) {
    	delete lwm2mclient;
    }

    SUITE_TEST_RESULT("test_bootstrap", interface_success && bootstrap_success);
    return interface_success && bootstrap_success;
}

int main() {
	bool result = true;

    MBED_HOSTTEST_TIMEOUT(20);
    MBED_HOSTTEST_SELECT(lwm2mclient_auto);
    MBED_HOSTTEST_DESCRIPTION(LWM2MClient Happy Day Test);
    MBED_HOSTTEST_START("LWM2MClientHappyDayTest");

    // This sets up the network interface configuration which will be used
    // by LWM2M Client API to communicate with mbed Device server.
    EthernetInterface eth;
    eth.init(); //Use DHCP
    eth.connect();

    lwipv4_socket_init();

    // Create test config object, and setup with unique MAC address
    TestConfig test_config;
    test_config.setup(eth.getMACAddress());

    result &= test_bootstrap(&test_config);
    result &= test_bootstrap(&test_config);
    result &= test_bootstrap(&test_config);
    result &= test_bootstrap(&test_config);
    result &= test_bootstrap(&test_config);
    result &= test_bootstrap(&test_config);

    SUITE_RESULT(result);
}

