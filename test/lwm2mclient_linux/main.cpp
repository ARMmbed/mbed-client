#include <unistd.h>
#include <pthread.h>
#include "m2minterfacefactory.h"
#include "m2minterface.h"
#include "m2mdevice.h"
#include "m2minterfaceobserver.h"
#include "m2mobjectinstance.h"
#include "m2mresource.h"

const String &BOOTSTRAP_SERVER_ADDRESS = "coap://10.45.3.10:5693";
const String &M2M_SERVER_ADDRESS = "coap://10.45.3.10:5683";
const String &MANUFACTURER = "manufacturer";
const String &TYPE = "type";
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
                                                  "test",
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
            printf("Waiting for bootstrap_successful callback\n");
            sleep(1);
        }
        return _bootstrapped;
    }

    bool register_successful() {
        while(!_registered && !_error) {
            printf("Waiting for register_successful callback\n");
            sleep(1);
        }
        return _registered;
    }

    bool unregister_successful() {
        while(!_unregistered && !_error) {
            sleep(1);
            printf("Waiting for unregister_successful callback\n");
        }
        return _unregistered;
    }

    bool registration_update_successful() {
        while(!_registration_updated && !_error) {
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
               _device->create_resource(M2MDevice::DeviceType,TYPE)        &&
               _device->create_resource(M2MDevice::ModelNumber,MODEL_NUMBER)      &&
               _device->create_resource(M2MDevice::SerialNumber,SERIAL_NUMBER)) {
                success = true;
            }
        }
        return success;
    }

    bool create_generic_object() {
        bool success = false;
        _object = M2MInterfaceFactory::create_object("Test");
        if(_object) {
            M2MObjectInstance* inst = _object->create_object_instance();
            if(inst) {
                    inst->create_static_resource("Test","R_test",value, sizeof(value));
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
        uint32_t updated_lifetime = 20;
        _registered = false;
        _unregistered = false;
        _interface->update_registration(_register_security,updated_lifetime);
    }

    void test_unregister() {
        _interface->unregister_object(NULL);
    }

    void bootstrap_done(M2MSecurity *server_object){
    if(server_object) {
            _register_security = server_object;
            _bootstrapped = true;
            printf("\nBootstrapped\n");
        }
    }

    void object_registered(M2MSecurity */*security_object*/, const M2MServer &/*server_object*/){
        _registered = true;
        printf("\nRegistered\n");
    }

    void object_unregistered(M2MSecurity */*server_object*/){
        _unregistered = true;
        printf("\nUnregistered\n");
    }

    void registration_updated(M2MSecurity */*security_object*/, const M2MServer & /*server_object*/){
        _registration_updated = true;
        printf("\nregistration updated\n");

    }

    void error(M2MInterface::Error error){
        _error = true;
        printf("\nError occured Error Code : %d\n", (int8_t)error);

    }

private:

    M2MInterface        *_interface;
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

void* wait_for_bootstrap(void* arg) {
    M2MLWClient *client;
    client = (M2MLWClient*) arg;
    if(client->bootstrap_successful()) {
        printf("Calling test_register\n");
        client->test_register();
    }
    return NULL;
}

void* wait_for_register(void* arg) {
    M2MLWClient *client;
    client = (M2MLWClient*) arg;
    if(client->register_successful()) {
        printf("Calling test_unregister\n");
        client->test_unregister();
    }
    return NULL;
}

void* wait_for_unregister(void* arg) {
    M2MLWClient *client;
    client = (M2MLWClient*) arg;
    if(client->unregister_successful()) {
        printf("unregister done\n");
    }
    return NULL;
}

int main() {

    pthread_t bootstrap_thread;
    pthread_t register_thread;
    pthread_t unregister_thread;
    M2MLWClient lwm2mclient;


    bool result = lwm2mclient.create_interface();
    if(true == result) {
        printf("\nInterface created\n");
    }
    result = lwm2mclient.create_bootstrap_object();
    if(true == result) {
        printf("Bootstrap object created");
    }

    result = lwm2mclient.create_device_object();
    if(true == result){
        printf("\nDevice object created !!\n");
    }

    result = lwm2mclient.create_generic_object();

    if(true == result) {
        printf("\nGeneric object created\n");
    }

    lwm2mclient.test_bootstrap();

    pthread_create(&bootstrap_thread, NULL, &wait_for_bootstrap, (void*) &lwm2mclient);
    pthread_create(&register_thread, NULL, &wait_for_register, (void*) &lwm2mclient);
    pthread_create(&unregister_thread, NULL, &wait_for_unregister, (void*) &lwm2mclient);

    pthread_join(bootstrap_thread, NULL);
    pthread_join(register_thread, NULL);
    pthread_join(unregister_thread, NULL);

    return 0;
}

