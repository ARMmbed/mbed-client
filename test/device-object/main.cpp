/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "mbed-net-sockets/UDPSocket.h"
#include "EthernetInterface.h"
#include "test_env.h"
#include "lwm2m-client/m2minterfacefactory.h"
#include "lwm2m-client/m2minterfaceobserver.h"
#include "lwm2m-client/m2mdevice.h"
#include "lwm2m-client/m2minterface.h"

// TODO: Remove when yotta supports init.
#include "lwipv4_init.h"

// Enter your mbed Device Server's IPv4 address and Port number in
// mentioned format like 192.168.0.1:5693
const String &M2M_SERVER_ADDRESS = "coap://10.45.3.10:5683";
const String &MANUFACTURER = "ARMFinland";

class M2MLWClient: public M2MInterfaceObserver {
public:
    M2MLWClient(){
        _interface = NULL;
        _register_security = NULL;
        _error = false;
        _registered = false;
        _unregistered = false;
        _registration_updated = false;
    }

    ~M2MLWClient() {
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
         _interface = M2MInterfaceFactory::create_interface(*this,
                                                   "lwm2m-test-endpoint",
                                                   "test",
                                                   3600,
                                                   5683,
                                                   "",
                                                   M2MInterface::UDP,
                                                   M2MInterface::LwIP_IPv4,
                                                   "");
         if (_interface) {
        	 success = true;
        	 printf("\nInterface created\n");
         }

         return success;
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
            security->set_resource_value(M2MSecurity::M2MServerUri, M2M_SERVER_ADDRESS);
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
    	}
    	return device;
    }

    void bootstrap_done(M2MSecurity *server_object){ }

    //Callback from mbed client stack when the registration
    // is successful, it returns the mbed Device Server object
    // to which the resources are registered and registered objects.
    void object_registered(M2MSecurity */*security_object*/, const M2MServer &/*server_object*/){
        _registered = true;
        _unregistered = false;
        printf("\nRegistered\n");
    }

    //Callback from mbed client stack when the registration update
        // is successful, it returns the mbed Device Server object
        // to which the resources are registered and registered objects.
    void registration_updated(M2MSecurity */*security_object*/, const M2MServer &/*server_object*/){
            _registration_updated = true;
            printf("\nUpdated\n");
        }

    //Callback from mbed client stack when the unregistration
    // is successful, it returns the mbed Device Server object
    // to which the resources were unregistered.
    void object_unregistered(M2MSecurity */*server_object*/){
        _unregistered = true;
        _registered = false;
        printf("\nUnregistered\n");
    }

//    void registration_updated(M2MSecurity */*security_object*/, const M2MServer & /*server_object*/){
//    	_registration_updated = true;
//    	printf("\nRegistration updated\n");
//    }

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
    volatile bool       _error;
    volatile bool       _registered;
    volatile bool       _unregistered;
    volatile bool       _registration_updated;
};

int main() {
	bool register_successful = false;
	bool update_successful = false;
	bool unregister_successful = false;

    MBED_HOSTTEST_TIMEOUT(50);
    MBED_HOSTTEST_SELECT(simple_auto);
    MBED_HOSTTEST_DESCRIPTION(LWM2MClient Device Object Test);
    MBED_HOSTTEST_START("LWM2MClientDeviceObjectTest");

    // This sets up the network interface configuration which will be used
    // by LWM2M Client API to communicate with mbed Device server.
    EthernetInterface eth;
    eth.init(); //Use DHCP
    eth.connect();

    lwipv4_socket_init();

    // Instantiate the class which implements
    // LWM2M Client API
    M2MLWClient lwm2mclient;

    // Create LWM2M Client API interface for M2M server
    bool interface_success = lwm2mclient.create_interface();

    M2MSecurity *register_object = lwm2mclient.create_register_object();

    lwm2mclient.set_register_object(register_object);

    // Create LWM2M device object specifying device resources
    // as per OMA LWM2M specification.
    M2MDevice* device_object = lwm2mclient.create_device_object();

    // Add the device object that we want to register
    // into the list and pass the list for register API.
    M2MObjectList object_list;
    object_list.push_back(device_object);

    // Issue register command.
    lwm2mclient.test_register(object_list);

    printf("test_register DONE\n");
    // Wait till the register callback is called successfully.
    // Callback comes in object_registered()
    while (!(register_successful = lwm2mclient.register_successful() ))
        { __WFI(); }

    printf("register callback DONE\n");
    // Wait 5 seconds
	wait_ms(1000);

	//TODO move this to callback when that can be taken in use
	update_successful = lwm2mclient.test_update_register(2222);

	printf("test_update_register DONE (callback is not checked yet)\n");

	// Wait till the register callback is called successfully.
    // Callback comes in object_updated()
	//while (!lwm2mclient.update_register_successful()) { __WFI(); }

	// Issue unregister command.
    lwm2mclient.test_unregister();

    // Wait for the unregister successful callback,
    // Callback comes in object_unregistered().
    while (!(unregister_successful = lwm2mclient.unregister_successful() ))
        { __WFI(); }

    // Delete device object created for registering device
    // resources.
    if(device_object) {
        delete device_object;
    }

    // Disconnect and teardown the network interface
    eth.disconnect();


    // Communicate test result
    //MBED_HOSTTEST_RESULT(interface_success &&
    //					 lwm2mclient.unregister_successful() &&
    //        			 lwm2mclient.register_successful());
    MBED_HOSTTEST_RESULT(interface_success &&
        			     register_successful &&
                		 unregister_successful);

}

