#include "mbed.h"
#include "UDPaSocket.h"
#include "socket_types.h"
#include "EthernetInterface.h"
#include "test_env.h"
#include "lwm2m-client/m2minterfacefactory.h"
#include "lwm2m-client/m2mdevice.h"
#include "lwm2m-client/m2minterfaceobserver.h"
#include "lwm2m-client/m2minterface.h"
#include "lwm2m-client/m2mobjectinstance.h"


// TODO: Remove when yotta supports init.
#include "lwipv4_init.h"

const String &BOOTSTRAP_SERVER_ADDRESS = "coap://10.45.3.10:5693";
const String &M2M_SERVER_ADDRESS = "coap://10.45.3.10:5683";
const String &MANUFACTURER = "arm";
const String &TYPE = "pressure";
const String &MODEL_NUMBER = "2015";
const String &SERIAL_NUMBER = "12345";

const uint8_t value[] = "MyValue";

class M2MLWClient: public M2MInterfaceObserver {
public:
    M2MLWClient(){
        _security = NULL;
        _interface = NULL;
        _device = NULL;
        _bootstrapped = false;
        _error = false;
        _registered = false;
        _unregistered = false;
        _registration_updated = false;
    }

    ~M2MLWClient() {
        if(_interface) {
            delete _interface;
        }
        if(_security) {
            delete _security;
        }
        if( _register_security){
            delete _register_security;
        }
    }

    bool create_interface() {
        _interface = M2MInterfaceFactory::create_interface(*this,
                                                  "lwm2m-endpoint",
                                                  "yogesh",
                                                  3600,
						  8000,
                                                  "",
                                                  M2MInterface::UDP,
                                                  M2MInterface::LwIP_IPv4,
                                                  "");
        return (_interface == NULL) ? false : true;
    }

    bool bootstrap_successful() {
        while(!_bootstrapped && !_error) {
            __WFI();
        }
        return _bootstrapped;
    }

    bool register_successful() {
        while(!_registered && !_error) {
            __WFI();
        }
        return _registered;
    }

    bool unregister_successful() {
        while(!_unregistered && !_error) {
            __WFI();
        }
        return _unregistered;
    }

    bool registration_update_successful() {
        while(!_registration_updated && !_error) {
            __WFI();
        }
        return _registration_updated;
    }

    bool create_bootstrap_object() {
        bool success = false;
        if(_security) {
            delete _security;
        }
        _security = M2MInterfaceFactory::create_security(M2MSecurity::Bootstrap);
        if(_security) {
            if(_security->set_resource_value(M2MSecurity::M2MServerUri, BOOTSTRAP_SERVER_ADDRESS) &&
            _security->set_resource_value(M2MSecurity::BootstrapServer, 1) &&
            _security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::NoSecurity)) {
                success = true;
                /* Not used now because there is no TLS or DTLS implementation available for stack.
                security->set_resource_value(M2MSecurity::ServerPublicKey,certificates->certificate_ptr[0],certificates->certificate_len[0]);
                security->set_resource_value(M2MSecurity::PublicKey,certificates->certificate_ptr[1],certificates->certificate_len[1]);
                security->set_resource_value(M2MSecurity::Secretkey,certificates->own_private_key_ptr,certificates->own_private_key_len);
                */
            }
        }
        return success;
    }

    bool create_register_object() {
        bool success = false;
        _register_security = M2MInterfaceFactory::create_security(M2MSecurity::M2MServer);
        if(_register_security) {
            if(_register_security->set_resource_value(M2MSecurity::M2MServerUri, M2M_SERVER_ADDRESS) &&
            _register_security->set_resource_value(M2MSecurity::BootstrapServer, 0) &&
            _register_security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::NoSecurity)) {
                success = true;
                /* Not used now because there is no TLS or DTLS implementation available for stack.
                security->set_resource_value(M2MSecurity::ServerPublicKey,certificates->certificate_ptr[0],certificates->certificate_len[0]);
                security->set_resource_value(M2MSecurity::PublicKey,certificates->certificate_ptr[1],certificates->certificate_len[1]);
                security->set_resource_value(M2MSecurity::Secretkey,certificates->own_private_key_ptr,certificates->own_private_key_len);
                */
            }
        }
        return success;
    }

    void test_bootstrap() {
        _interface->bootstrap(_security);
    }

    bool create_device_object() {
        bool success = false;
        _device = M2MInterfaceFactory::create_device();
        if(_device) {
            if(_device->create_resource(M2MDevice::Manufacturer,MANUFACTURER)     &&
               _device->create_resource(M2MDevice::DeviceType,TYPE)        	  &&
               _device->create_resource(M2MDevice::ModelNumber,MODEL_NUMBER)      &&
               _device->create_resource(M2MDevice::SerialNumber,SERIAL_NUMBER)) {
                success = true;
            }
        }
        return success;
    }

    bool create_generic_object() {
        bool success = false;
        _object = M2MInterfaceFactory::create_object("Yogesh");
        if(_object) {
            M2MObjectInstance* inst = _object->create_object_instance();
            if(inst) {
                    inst->create_static_resource("Test","R_test",value, sizeof(value)-1);
                    success = true;
                }
        }
        return success;
    }

    void test_register(){
        M2MObjectList object_list;
        object_list.push_back(_device);
	object_list.push_back(_object);

        _interface->register_object(_register_security,object_list);
    }

    void test_update_register() {
        uint32_t updated_lifetime = 120;
        _registered = false;
        _unregistered = false;
        _interface->update_registration(updated_lifetime);
    }

    void test_unregister() {
        _interface->unregister_object(NULL);
    }

    void bootstrap_done(M2MSecurity *server_object){
    if(server_object) {
            _register_security = server_object;
            _bootstrapped = true;
            _error = false;
            printf("\nBootstrapped\n");
        }
    }

    void object_registered(/*M2MSecurity *security_object, const M2MServer &server_object*/){
        _registered = true;
        _unregistered = false;
        printf("\nRegistered\n");
    }

    void object_unregistered(M2MSecurity */*server_object*/){
        _unregistered = true;
        _registered = false;
        printf("\nUnregistered\n");
    }

    void registration_updated(M2MSecurity */*security_object*/, const M2MServer & /*server_object*/){
        _registration_updated = true;
        _unregistered = false;
        printf("\nregistration updated\n");

    }

    void error(M2MInterface::Error /*error*/){
        _error = true;
        _bootstrapped = false;
        printf("\nError occured\n");
    }

private:

    M2MInterface    	*_interface;
    M2MSecurity         *_security;
    M2MSecurity         *_register_security;
    M2MDevice           *_device;
    M2MObject           *_object;
    bool                _bootstrapped;
    bool                _error;
    bool                _registered;
    bool                _unregistered;
    bool                _registration_updated;
};

int main() {
    EthernetInterface eth;
    eth.init(); //Use DHCP
    eth.connect();

    // TODO: Remove when yotta supports init
    lwipv4_socket_init();

    M2MLWClient lwm2mclient;

    char result[20];

    notify_completion_str(lwm2mclient.create_interface(),result);
    printf("Interface creation test case  %s !!", result);

    notify_completion_str(lwm2mclient.create_bootstrap_object(),result);
    printf("Bootstrap object creation test case  %s !!", result);

    lwm2mclient.test_bootstrap();
    notify_completion_str(lwm2mclient.bootstrap_successful(),result);
    printf("Bootstrap test case  %s !!", result);

    notify_completion_str(lwm2mclient.create_device_object(),result);
    printf("Device object creation test case  %s !!", result);

    lwm2mclient.test_register();
    notify_completion_str(lwm2mclient.register_successful(),result);
    printf("Register test case  %s !!", result);

   /* lwm2mclient.test_update_register();
    notify_completion_str(lwm2mclient.registration_update_successful(),result);
    printf("Update Registration test case  %s !!", result);*/

    lwm2mclient.test_unregister();
    notify_completion_str(lwm2mclient.unregister_successful(),result);
    printf("Unregister test case  %s !!", result);

    eth.disconnect();

    return 0;
}

