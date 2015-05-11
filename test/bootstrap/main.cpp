/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "mbed-net-sockets/UDPSocket.h"
#include "EthernetInterface.h"
#include "test_env.h"
#include "lwm2m-client/m2minterfacefactory.h"
#include "lwm2m-client/m2minterfaceobserver.h"
#include "lwm2m-client/m2minterface.h"

// TODO: Remove when yotta supports init.
#include "lwipv4_init.h"

// Enter your mbed Device Server's IPv4 address and Port number in
// mentioned format like 192.168.0.1:5693
const String &BOOTSTRAP_SERVER_ADDRESS = "coap://10.45.3.10:5693";

class M2MLWClient: public M2MInterfaceObserver {
public:
    M2MLWClient(){
        _interface = NULL;
        _bootstrapped = false;
        _error = false;
    }

    ~M2MLWClient() {
        if(_interface) {
            delete _interface;
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

    bool bootstrap_successful() {
        return _bootstrapped;
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

    void test_bootstrap(M2MSecurity *security) {
        if(_interface) {
             // Bootstrap function.
            _interface->bootstrap(security);
        }
    }


    //Callback from mbed client stack when the bootstrap
    // is successful, it returns the mbed Device Server object
    // which will be used for registering the resources to
    // mbed Device server.
    void bootstrap_done(M2MSecurity *server_object){
    	if(server_object) {
            _bootstrapped = true;
            _error = false;
            printf("\nBootstrapped\n");
        }
    }

    void object_registered(M2MSecurity */*security_object*/, const M2MServer &/*server_object*/){
    }

    void object_unregistered(M2MSecurity */*server_object*/){
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
    volatile bool       _bootstrapped;
    volatile bool       _error;
};

int main() {
    MBED_HOSTTEST_TIMEOUT(20);
    MBED_HOSTTEST_SELECT(simple_auto);
    MBED_HOSTTEST_DESCRIPTION(LWM2MClient Bootstrap Test);
    MBED_HOSTTEST_START("LWM2MClientBootstrapTest");

    // This sets up the network interface configuration which will be used
    // by LWM2M Client API to communicate with mbed Device server.
    EthernetInterface eth;
    eth.init(); //Use DHCP
    eth.connect();

    lwipv4_socket_init();


    // Instantiate the class which implements
    // LWM2M Client API
    M2MLWClient lwm2mclient;

    // Create LWM2M Client API interface to manage bootstrap,
    // register and unregister
    bool interface_success = lwm2mclient.create_interface();

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

    // Disconnect the connect and teardown the network interface
    eth.disconnect();

    MBED_HOSTTEST_RESULT(interface_success && lwm2mclient.bootstrap_successful());
}

