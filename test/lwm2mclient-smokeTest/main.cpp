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

//TODO: should this be configured in .json conf file, and communicated via host test to here?
int CALLBACK_TIMEOUT = 5;

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
                    M2MResource* res = inst->create_dynamic_resource("D","ResourceTest",
                                                                     M2MResourceInstance::INTEGER,
                                                                     true);
                    char buffer[20];
                    int size = sprintf(buffer,"%d",_resource_value);
                    res->set_operation(M2MBase::GET_PUT_POST_ALLOWED);
                    res->set_value((const uint8_t*)buffer,
                                   (const uint32_t)size);
                    _resource_value++;

                    inst->create_static_resource("S",
                                                 "ResourceTest",
                                                 M2MResourceInstance::STRING,
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
#define SUITE_TEST_RESULT(test_name, result)	printf("Suite-%s: result %s\n", test_name, result ? "PASSED" : "FAILED")
#define SUITE_RESULT(result)					printf("Suite: result %s\n", result ? "success" : "failure")


#define WAIT_CALLBACK(X, TIMEOUT)                                  \
{int _timer = 0;\
while ( 1 )       \
    { 													  \
      _result &= (X);									  \
      if (_result) {									  \
          SUITE_TEST_INFO(_tn, "callback done");          \
          break;										  \
      }													  \
      wait_ms(1000); _timer+=1;                           \
      if (_timer >= TIMEOUT) {                            \
          SUITE_TEST_INFO(_tn, "ERROR: callback timeout");\
          break;                                          \
      }                                                   \
    }}

bool test_bootStrap(TestConfig *test_config) {
    bool _result = true;
    const char* _tn = "TC1_bootStrap";

    SUITE_TEST_INFO(_tn, "STARTED");

    // Instantiate the class which implements
    // LWM2M Client API
    M2MLWClient *lwm2mclient = new M2MLWClient(test_config);

    SUITE_TEST_INFO(_tn, "client done");

    // Create LWM2M Client API interface to manage bootstrap,
    // register and unregister
    _result &= lwm2mclient->create_interface();

    // Create LWM2M bootstrap object specifying bootstrap server
    // information.
    M2MSecurity* security_object = lwm2mclient->create_bootstrap_object();

    // Issue bootstrap command.
    lwm2mclient->test_bootstrap(security_object);
    SUITE_TEST_INFO(_tn, "bootstrap done");

    SUITE_TEST_INFO(_tn, "waiting bootstrap callback...");
    // Wait till the bootstrap callback is called successfully.
    // Callback comes in bootstrap_done()
    WAIT_CALLBACK(lwm2mclient->bootstrap_successful(), CALLBACK_TIMEOUT);

    // Delete security object created for bootstrapping
    if(security_object) {
        delete security_object;
    }

    if (lwm2mclient) {
        delete lwm2mclient;
    }

    SUITE_TEST_RESULT(_tn, _result);
    return _result;
}

bool test_deviceObject(TestConfig *test_config) {
    bool _result = true;
    const char* _tn = "TC2_deviceObject";

    SUITE_TEST_INFO(_tn, "STARTED");

    // Instantiate the class which implements
    // LWM2M Client API
    M2MLWClient *lwm2mclient = new M2MLWClient(test_config);

    SUITE_TEST_INFO(_tn, "client done");

    // Create LWM2M Client API interface for M2M server
    _result &= lwm2mclient->create_interface();

    M2MSecurity *register_object = lwm2mclient->create_register_object();

    lwm2mclient->set_register_object(register_object);

    // Create LWM2M device object specifying device resources
    // as per OMA LWM2M specification.
    M2MDevice* device_object = lwm2mclient->create_device_object();

    // Add the device object that we want to register
    // into the list and pass the list for register API.
    M2MObjectList object_list;
    object_list.push_back(device_object);

    // Issue register command.
    lwm2mclient->test_register(object_list);
    SUITE_TEST_INFO(_tn, "register done");

    SUITE_TEST_INFO(_tn, "waiting register callback...");
    wait_ms(1000);
    // Callback comes in object_registered()
    WAIT_CALLBACK(lwm2mclient->register_successful(), CALLBACK_TIMEOUT);
    //SUITE_TEST_INFO(_tn, "register callback done");

    // Wait 5 seconds
    wait_ms(1000);

    //TODO move this to callback when that can be taken in use
    _result &= lwm2mclient->test_update_register(2222);
    SUITE_TEST_INFO(_tn, "update register done");

    //SUITE_TEST_INFO(_tn, "waiting update register callback...");
    // Callback comes in object_updated()
    //WAIT_CALLBACK(lwm2mclient->update_register_successful(), CALLBACK_TIMEOUT);

    // Issue unregister command.
    lwm2mclient->test_unregister();
    SUITE_TEST_INFO(_tn, "unregister done");

    SUITE_TEST_INFO(_tn, "waiting unregister callback...");
    // Callback comes in object_unregistered().
    WAIT_CALLBACK(lwm2mclient->unregister_successful(), CALLBACK_TIMEOUT);

    // Delete device object created for registering device
    // resources.
    if(device_object) {
        M2MDevice::delete_instance();
    }

    if (lwm2mclient) {
        delete lwm2mclient;
    }

    SUITE_TEST_RESULT(_tn, _result);
    return _result;

}

bool test_resource(TestConfig *test_config) {
    bool _result = true;
    const char* _tn = "TC3_resource";
    SUITE_TEST_INFO(_tn, "STARTED");

    // Instantiate the class which implements LWM2M Client API
    M2MLWClient *lwm2mclient = new M2MLWClient(test_config);
    SUITE_TEST_INFO(_tn, "client done");

    // Create LWM2M Client API interface for M2M server
    _result &= lwm2mclient->create_interface();

    M2MSecurity *register_object = lwm2mclient->create_register_object();

    lwm2mclient->set_register_object(register_object);

    // Create LWM2M device object specifying device resources
    // as per OMA LWM2M specification.
    M2MDevice* device_object = lwm2mclient->create_device_object();

    // Create LWM2M generic object for resource
    M2MObject* resource_object = lwm2mclient->create_generic_object();

    // Add the device object that we want to register
    // into the list and pass the list for register API.
    M2MObjectList object_list;
    object_list.push_back(device_object);
    object_list.push_back(resource_object);

    // Issue register command.
    lwm2mclient->test_register(object_list);
    SUITE_TEST_INFO(_tn, "register done");

    SUITE_TEST_INFO(_tn, "waiting register callback...");
    // Callback comes in object_registered()
    WAIT_CALLBACK(lwm2mclient->register_successful(), CALLBACK_TIMEOUT);

    // Wait 5 seconds
    wait_ms(5000);

    // Issue unregister command.
    lwm2mclient->test_unregister();
    SUITE_TEST_INFO(_tn, "unregister done");

    SUITE_TEST_INFO(_tn, "waiting unregister callback...");
    // Callback comes in object_unregistered().
    WAIT_CALLBACK(lwm2mclient->unregister_successful(), CALLBACK_TIMEOUT);

    // Delete device object created for registering device resources.
    if(device_object) {
        M2MDevice::delete_instance();
    }

    // Delete resource object for registering resources.
    if(resource_object) {
        delete resource_object;
    }

    if (lwm2mclient) {
        delete lwm2mclient;
    }

    SUITE_TEST_RESULT(_tn, _result);
    return _result;
}



void app_start(int /*argc*/, char* /*argv*/[]) {
    bool result = true;
    DigitalOut _led = DigitalOut(LED3);
    _led = 1;

    MBED_HOSTTEST_TIMEOUT(40);
    MBED_HOSTTEST_SELECT(lwm2mclient_auto);
    MBED_HOSTTEST_DESCRIPTION(LWM2MClient Smoke Test);
    MBED_HOSTTEST_START("LWM2MClientSmokeTest");

    // This sets up the network interface configuration which will be used
    // by LWM2M Client API to communicate with mbed Device server.
    EthernetInterface eth;
    eth.init(); //Use DHCP
    eth.connect();

    lwipv4_socket_init();

    // Create test config object, and setup with unique MAC address
    TestConfig test_config;
    test_config.setup();

    _led = 0;

    result &= test_bootStrap(&test_config);
    result &= test_deviceObject(&test_config);
    result &= test_resource(&test_config);

    _led = 1;

    // Disconnect and teardown the network interface
    eth.disconnect();

    // Communicate test result
    SUITE_RESULT(result);
}

