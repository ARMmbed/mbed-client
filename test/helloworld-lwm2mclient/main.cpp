#include "mbed.h"
#include "UDPaSocket.h"
#include "socket_types.h"
#include "EthernetInterface.h"
#include "test_env.h"
#include "m2minterfacefactory.h"
#include "m2mdevice.h"
#include "m2minterfaceobserver.h"

// TODO: Remove when yotta supports init.
#include "lwipv4_init.h"

const String &BOOTSTRAP_SERVER_ADDRESS = "coap://10.45.3.42:5685";

class M2MLWClient: public M2MInterfaceObserver {
public:
    M2MLWClient(){}

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
                                                  "node-001",
                                                  "test",
                                                  60,
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

    bool create_security_object() {
        bool success = false;
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

    void test_bootstrap() {
        _interface->bootstrap(_security);
    }

    bool create_device_object() {
        bool success = false;
        _device = M2MInterfaceFactory::create_device();
        if(_device) {
            if(_device->create_resource(M2MDevice::Manufacturer,"test_manufacturer")     &&
               _device->create_resource(M2MDevice::DeviceType,"test_device_type")        &&
               _device->create_resource(M2MDevice::ModelNumber,"test_model_number")      &&
               _device->create_resource(M2MDevice::SerialNumber,"test_serial_number")    &&
               _device->create_resource(M2MDevice::Manufacturer,"test_manufacturer")) {
                success = true;
            }
        }
        return success;
    }

    void test_register(){
        M2MObjectList object_list;
        object_list.push_back(_device);

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
        }
    }

    void object_registered(/*M2MSecurity *security_object, const M2MServer &server_object*/){
        _registered = true;
        _unregistered = false;

    }
    void object_unregistered(M2MSecurity */*server_object*/){
        _unregistered = true;
        _registered = false;
    }

    void registration_updated(M2MSecurity */*security_object*/, const M2MServer & /*server_object*/){
        _registration_updated = true;
        _unregistered = false;
    }

    void error(M2MInterface::Error /*error*/){
        _error = true;
        _bootstrapped = false;
    }

private:

    M2MInterfaceImpl    *_interface;
    M2MSecurity         *_security;
    M2MSecurity         *_register_security;
    M2MDevice           *_device;
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

    notify_completion_str(lwm2mclient.create_security_object(),result);
    printf("Security object creation test case  %s !!", result);

    lwm2mclient.test_bootstrap();
    notify_completion_str(lwm2mclient.bootstrap_successful(),result);
    printf("Bootstrap test case  %s !!", result);

    notify_completion_str(lwm2mclient.create_device_object(),result);
    printf("Device object creation test case  %s !!", result);

    lwm2mclient.test_register();
    notify_completion_str(lwm2mclient.register_successful(),result);
    printf("Register test case  %s !!", result);

    lwm2mclient.test_update_register();
    notify_completion_str(lwm2mclient.registration_update_successful(),result);
    printf("Update Registration test case  %s !!", result);

    lwm2mclient.test_unregister();
    notify_completion_str(lwm2mclient.unregister_successful(),result);
    printf("Unregister test case  %s !!", result);

    eth.disconnect();

    return 0;
}

