/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "mbed-net-sockets/UDPSocket.h"
#include "EthernetInterface.h"
#include "test_env.h"
#include "lwm2m-client/m2minterfacefactory.h"
#include "lwm2m-client/m2mdevice.h"
#include "lwm2m-client/m2minterfaceobserver.h"
#include "lwm2m-client/m2minterface.h"
#include "lwm2m-client/m2mobjectinstance.h"

// TODO: Remove when yotta supports init.
#include "lwipv4_init.h"

#define BOOTSTRAP_ENABLED

// Enter your mbed Device Server's IPv4 address and Port number in
// mentioned format like 192.168.0.1:5693
const String &BOOTSTRAP_SERVER_ADDRESS = "coap://10.45.3.10:5693";
const String &MBED_SERVER_ADDRESS = "coap://10.45.3.10:5683";
const String &MANUFACTURER = "ARM";
const String &TYPE = "type";
const String &MODEL_NUMBER = "2015";
const String &SERIAL_NUMBER = "12345";

#if defined(TARGET_K64F)
#define OBS_BUTTON SW2
#define UNREG_BUTTON SW3
#endif

class M2MLWClient: public M2MInterfaceObserver {
public:
    M2MLWClient(){
        _interface = NULL;
        _bootstrapped = false;
        _error = false;
        _registered = false;
        _unregistered = false;
        _register_security = NULL;
        _value = 0;
        _object = NULL;
    }

    ~M2MLWClient() {
        if(_interface) {
            delete _interface;
        }
        if( _register_security){
            delete _register_security;
        }
    }

    void create_interface() {
        // Creates M2MInterface using which endpoint can
        // setup its name, resource type, life time, connection mode,
        // Currently only LwIPv4 is supported.
        _interface = M2MInterfaceFactory::create_interface(*this,
                                                  "lwm2m-endpoint",
                                                  "test",
                                                  3600,
                                                  5683,
                                                  "",
                                                  M2MInterface::UDP,
                                                  M2MInterface::LwIP_IPv4,
                                                  "");
    }

    bool bootstrap_successful() {
        return _bootstrapped;
    }

    bool register_successful() {
        return _registered;
    }

    bool unregister_successful() {
        return _unregistered;
    }

    M2MSecurity* create_bootstrap_object() {
        // Creates bootstrap server object with Bootstrap server address and other parameters
        // required for client to connect to bootstrap server.
        M2MSecurity *security = M2MInterfaceFactory::create_security(M2MSecurity::Bootstrap);
        if(security) {
            security->set_resource_value(M2MSecurity::M2MServerUri, BOOTSTRAP_SERVER_ADDRESS);
            security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::NoSecurity);
        }
        return security;
    }

    M2MSecurity* create_register_object() {
        // Creates bootstrap server object with Bootstrap server address and other parameters
        // required for client to connect to bootstrap server.
        M2MSecurity *security = M2MInterfaceFactory::create_security(M2MSecurity::M2MServer);
        if(security) {
            security->set_resource_value(M2MSecurity::M2MServerUri, MBED_SERVER_ADDRESS);
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

    M2MDevice* create_device_object() {
        // Creates device object which contains mandatory resources linked with
        // device endpoint.
        M2MDevice *device = M2MInterfaceFactory::create_device();
        if(device) {
            device->create_resource(M2MDevice::Manufacturer,MANUFACTURER);
            device->create_resource(M2MDevice::DeviceType,TYPE);
            device->create_resource(M2MDevice::ModelNumber,MODEL_NUMBER);
            device->create_resource(M2MDevice::SerialNumber,SERIAL_NUMBER);
        }
        return device;
    }

    M2MObject* create_generic_object() {
        _object = M2MInterfaceFactory::create_object("Test");
        if(_object) {
            M2MObjectInstance* inst = _object->create_object_instance();
            if(inst) {
                    M2MResource* res = inst->create_dynamic_resource("Test","ResourceTest",true);
                    char buffer[20];
                    int size = sprintf(buffer,"%d",_value);
                    res->set_operation(M2MBase::GET_PUT_POST_ALLOWED);
                    res->set_value((const uint8_t*)buffer,
                                   (const uint32_t)size);
                    _value++;
                }
        }
        return _object;
    }

    void update_resource() {
        if(_object) {
            M2MObjectInstance* inst = _object->object_instance();
            if(inst) {
                    M2MResource* res = inst->resource("Test");

                    char buffer[20];
                    int size = sprintf(buffer,"%d",_value);
                    res->set_value((const uint8_t*)buffer,
                                   (const uint32_t)size);
                    _value++;
                }
        }
    }

    void test_register(M2MObjectList object_list){
        if(_interface) {
            // Register function
            _interface->register_object(_register_security, object_list);
        }
    }

    void set_register_object(M2MSecurity *&register_object){
        if(_register_security) {
            delete _register_security;
            _register_security = NULL;
        }
        _register_security = register_object;

    }

    void test_unregister() {
        if(_interface) {
            // Unregister function
            _interface->unregister_object(NULL);
        }
    }

    //Callback from mbed client stack when the bootstrap
    // is successful, it returns the mbed Device Server object
    // which will be used for registering the resources to
    // mbed Device server.
    void bootstrap_done(M2MSecurity *server_object){
    if(server_object) {
            set_register_object(server_object);
            _bootstrapped = true;
            _error = false;
            printf("\nBootstrapped\n");
        }
    }

    //Callback from mbed client stack when the registration
    // is successful, it returns the mbed Device Server object
    // to which the resources are registered and registered objects.
    void object_registered(M2MSecurity */*security_object*/, const M2MServer &/*server_object*/){
        _registered = true;
        _unregistered = false;
        printf("\nRegistered\n");
    }

    //Callback from mbed client stack when the unregistration
    // is successful, it returns the mbed Device Server object
    // to which the resources were unregistered.
    void object_unregistered(M2MSecurity */*server_object*/){
        _unregistered = true;
        _registered = false;
        printf("\nUnregistered\n");
    }

    void registration_updated(M2MSecurity */*security_object*/, const M2MServer & /*server_object*/){
    }

    //Callback from mbed client stack if any error is encountered
    // during any of the LWM2M operations. Error type is passed in
    // the callback.
    void error(M2MInterface::Error /*error*/){
        _error = true;
        printf("\nError occured\n");
    }

private:

    M2MInterface    	*_interface;
    M2MSecurity         *_register_security;
    M2MObject           *_object;
    volatile bool       _bootstrapped;
    volatile bool       _error;
    volatile bool       _registered;
    volatile bool       _unregistered;
    int                 _value;
};

int main() {

    // This sets up the network interface configuration which will be used
    // by LWM2M Client API to communicate with mbed Device server.
    EthernetInterface eth;
    eth.init(); //Use DHCP
    eth.connect();

    lwipv4_socket_init();


    // Instantiate the class which implements
    // LWM2M Client API
    M2MLWClient lwm2mclient;

    // Set up Hardware interrupt button.
    InterruptIn obs_button(OBS_BUTTON);

    InterruptIn unreg_button(UNREG_BUTTON);

    // On press of SW3 button on K64F board, example application
    // will call unregister API towards mbed Device Server
    unreg_button.fall(&lwm2mclient,&M2MLWClient::test_unregister);

    // On press of SW2 button on K64F board, example application
    // will send observation towards mbed Device Server
    obs_button.fall(&lwm2mclient,&M2MLWClient::update_resource);

    // Create LWM2M Client API interface to manage bootstrap,
    // register and unregister
    lwm2mclient.create_interface();

#if defined (BOOTSTRAP_ENABLED)
    // Create LWM2M bootstrap object specifying bootstrap server
    // information.
    M2MSecurity* security_object = lwm2mclient.create_bootstrap_object();

    // Issue bootstrap command.
    lwm2mclient.test_bootstrap(security_object);

    // Wait till the bootstrap callback is called successfully.
    // Callback comes in bootstrap_done()
    while (!lwm2mclient.bootstrap_successful()) { __WFI(); }

    // Delete security object created for bootstrapping
    if(security_object) {
        delete security_object;
    }
#else

    M2MSecurity *register_object = lwm2mclient.create_register_object();

    lwm2mclient.set_register_object(register_object);

#endif

    // Create LWM2M device object specifying device resources
    // as per OMA LWM2M specification.
    M2MDevice* device_object = lwm2mclient.create_device_object();

    M2MObject* object = lwm2mclient.create_generic_object();

    // Add all the objects that you would like to register
    // into the list and pass the list for register API.
    M2MObjectList object_list;
    object_list.push_back(device_object);
    object_list.push_back(object);

    // Issue register command.
    lwm2mclient.test_register(object_list);

    // Wait till the register callback is called successfully.
    // Callback comes in object_registered()
    while (!lwm2mclient.register_successful()) { __WFI(); }

    // Wait for the unregister successful callback,
    // Callback comes in object_unregsitered(), this will be
    // waiting for user to press SW2 button on K64F board.
    while (!lwm2mclient.unregister_successful()) { __WFI(); }

#if defined(BOOTSTRAP_ENABLED)
    // This will turn on the LED on the board specifying that
    // the application has run successfully.
    notify_completion(lwm2mclient.unregister_successful() &&
                      lwm2mclient.register_successful() &&
                      lwm2mclient.bootstrap_successful());
#else

    // This will turn on the LED on the board specifying that
    // the application has run successfully.
    notify_completion(lwm2mclient.unregister_successful() &&
                      lwm2mclient.register_successful());

#endif

    // Delete device object created for registering device
    // resources.
    if(device_object) {
        delete device_object;
    }
    if(object) {
        delete object;
    }

    // Disconnect the connect and teardown the network interface
    eth.disconnect();

    return 0;
}

