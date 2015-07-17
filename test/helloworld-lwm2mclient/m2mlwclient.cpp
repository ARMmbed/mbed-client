
#include "m2mlwclient.h"

#include "lwm2m-client/m2minterfacefactory.h"
#include "lwm2m-client/m2mdevice.h"
#include "lwm2m-client/m2mobjectinstance.h"
#include "lwm2m-client/m2mresource.h"

// Enter your mbed Device Server's IPv4 address and Port number in
// mentioned format like 192.168.0.1:5693
const String &BOOTSTRAP_SERVER_ADDRESS = "coap://10.45.3.10:5693";
const String &BOOTSTRAP_SERVER_DTLS_ADDRESS = "coap://10.45.3.10:5694";

#ifdef SIXLOWPAN_INTERFACE
const String &MBED_SERVER_ADDRESS = "coap://FD00:FF1:CE0B:A5E1:1068:AF13:9B61:D557:5683";
const String &MBED_SERVER_DTLS_ADDRESS = "coap://FD00:FF1:CE0B:A5E1:1068:AF13:9B61:D557:5684";
#else
const String &MBED_SERVER_ADDRESS = "coap://10.45.3.10:5683";
const String &MBED_SERVER_DTLS_ADDRESS = "coap://10.45.3.10:5684";
#endif

const uint16_t SERVER_PORT = 5683;
const uint16_t SECURE_PORT = 5683;

const String &MANUFACTURER = "ARM";
const String &TYPE = "type";
const String &MODEL_NUMBER = "2015";
const String &SERIAL_NUMBER = "12345";
const uint8_t STATIC_VALUE[] = "Static value";

#ifdef TARGET_LIKE_LINUX
#include <unistd.h>
#else

#endif

M2MLWClient::M2MLWClient()
#ifndef TARGET_LIKE_LINUX
:_led(LED3)
#endif
{
    _security = NULL;
    _interface = NULL;
    _register_security = NULL;
    _device = NULL;
    _object = NULL;
    _bootstrapped = false;
    _error = false;
    _registered = false;
    _unregistered = false;
    _registration_updated = false;
    _value = 0;
}

M2MLWClient::~M2MLWClient() {
    if(_security) {
        delete _security;
    }
    if(_register_security){
        delete _register_security;
    }
    if(_device) {
        M2MDevice::delete_instance();
    }
    if(_object) {
        delete _object;
    }
    if(_interface) {
        delete _interface;
    }
}

bool M2MLWClient::create_interface(bool useSecureConnection) {

    M2MInterface::NetworkStack stack = M2MInterface::LwIP_IPv4;
    #ifdef SIXLOWPAN_INTERFACE
            stack = M2MInterface::Nanostack_IPv6;
    #endif

    if(useSecureConnection){
        _interface = M2MInterfaceFactory::create_interface(*this,
                                                  "client-endpoint",
                                                  "test",
                                                  60,
                                                  SECURE_PORT,
                                                  "",
                                                  M2MInterface::UDP,
                                                  stack,
                                                  "");
        printf("Endpoint Name : linux-secure-endpoint\n");
    }else{
        _interface = M2MInterfaceFactory::create_interface(*this,
                                                  "lwm2m-endpoint",
                                                  "test",
                                                  60,
                                                  SERVER_PORT,
                                                  "",
                                                  M2MInterface::UDP,
                                                  stack,
                                                  "");
        printf("Endpoint Name : lwm2m-endpoint\n");
    }

    return (_interface == NULL) ? false : true;
}

bool M2MLWClient::bootstrap_successful() {
#ifdef TARGET_LIKE_LINUX
    while(!_bootstrapped && !_error) {
        sleep(1);
    }
#endif
    return _bootstrapped;
}

bool M2MLWClient::register_successful() {
#ifdef TARGET_LIKE_LINUX
    while(!_registered && !_error) {
        sleep(1);
    }
#endif
    return _registered;
}

bool M2MLWClient::unregister_successful() {
#ifdef TARGET_LIKE_LINUX
    while(!_unregistered && !_error) {
        sleep(1);
    }
#endif
    return _unregistered;
}

bool M2MLWClient::registration_update_successful() {
#ifdef TARGET_LIKE_LINUX
    while(!_registration_updated && !_error) {
        sleep(1);
    }
#endif
    return _registration_updated;
}

M2MSecurity* M2MLWClient::create_bootstrap_object(bool useSecureConnection) {
    // Creates bootstrap server object with Bootstrap server address and other parameters
    // required for client to connect to bootstrap server.
    M2MSecurity *security = M2MInterfaceFactory::create_security(M2MSecurity::Bootstrap);
    if(security) {
        if(useSecureConnection){
            security->set_resource_value(M2MSecurity::M2MServerUri, BOOTSTRAP_SERVER_DTLS_ADDRESS);
            //TODO: remove these, when bootstrapping server supports DTLS
            delete security;
            return NULL;
        }else{
            security->set_resource_value(M2MSecurity::M2MServerUri, BOOTSTRAP_SERVER_ADDRESS);
            //TODO: refactor this out
            security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::NoSecurity);
        }
    }
    return security;
}

M2MSecurity* M2MLWClient::create_register_object(bool useSecureConnection) {
    // Creates bootstrap server object with Bootstrap server address and other parameters
    // required for client to connect to bootstrap server.
    M2MSecurity *security = M2MInterfaceFactory::create_security(M2MSecurity::M2MServer);
    if(security) {
        if(useSecureConnection){
            security->set_resource_value(M2MSecurity::M2MServerUri, MBED_SERVER_DTLS_ADDRESS);
        }else{
            security->set_resource_value(M2MSecurity::M2MServerUri, MBED_SERVER_ADDRESS);
        }
    }
    return security;
}

void M2MLWClient::test_bootstrap(M2MSecurity *security) {
    if(_interface) {
         // Bootstrap function.
        _interface->bootstrap(security);
    }
}

M2MDevice* M2MLWClient::create_device_object() {
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

void M2MLWClient::execute_function(void *argument) {
#ifdef TARGET_LIKE_LINUX
    if(argument) {
        char* arguments = (char*)argument;
        printf("Received %s!!\n", arguments);
    }
    printf("I am executed !!\n");
#else
    _led == 0 ? _led = 1 : _led = 0;
#endif
}

M2MObject* M2MLWClient::create_generic_object() {
    _object = M2MInterfaceFactory::create_object("Test");
    if(_object) {
        M2MObjectInstance* inst = _object->create_object_instance();
        if(inst) {
            M2MResource* res = inst->create_dynamic_resource("Dynamic","ResourceTest",true);
            char buffer[20];
            int size = sprintf(buffer,"%d",_value);
            res->set_operation(M2MBase::GET_PUT_POST_ALLOWED);
            res->set_value((const uint8_t*)buffer,
                           (const uint32_t)size);
            res->set_execute_function(execute_callback(this,&M2MLWClient::execute_function));
            _value++;

            inst->create_static_resource("Static",
                                         "ResourceTest",
                                         STATIC_VALUE,
                                         sizeof(STATIC_VALUE)-1);
        }
    }
    return _object;
}

void M2MLWClient::update_resource() {
    if(_object) {
        M2MObjectInstance* inst = _object->object_instance();
        if(inst) {
            M2MResource* res = inst->resource("Dynamic");
            char buffer[20];
            int size = sprintf(buffer,"%d",_value);
            res->set_value((const uint8_t*)buffer,
                           (const uint32_t)size,
                           true);
            _value++;
        }
    }
}

void M2MLWClient::test_register(M2MObjectList object_list){
    if(_interface) {
        _interface->register_object(_register_security, object_list);
    }
}

void M2MLWClient::set_register_object(M2MSecurity *&register_object){
    if(_register_security) {
        delete _register_security;
        _register_security = NULL;
    }
    _register_security = register_object;

}

void M2MLWClient::test_update_register() {
    uint32_t updated_lifetime = 20;
    _registered = false;
    _unregistered = false;
    if(_interface){
        _interface->update_registration(_register_security,updated_lifetime);
    }
}

void M2MLWClient::test_unregister() {
    if(_interface) {
        _interface->unregister_object(NULL);
    }
}

void M2MLWClient::bootstrap_done(M2MSecurity *server_object){
    if(server_object) {
        set_register_object(server_object);
        _bootstrapped = true;
        _error = false;
        printf("\nBootstrapped\n");
    }
}

void M2MLWClient::object_registered(M2MSecurity */*security_object*/, const M2MServer &/*server_object*/){
    _registered = true;
    _unregistered = false;
    printf("\nRegistered\n");
}

void M2MLWClient::object_unregistered(M2MSecurity */*server_object*/){
    _unregistered = true;
    _registered = false;
    printf("\nUnregistered\n");
}

void M2MLWClient::registration_updated(M2MSecurity */*security_object*/, const M2MServer & /*server_object*/){
    _registration_updated = true;
    printf("\nregistration updated\n");
}

void M2MLWClient::error(M2MInterface::Error error){
    _error = true;
    printf("\nError occured Error code %d\n", (int)error);
}

void M2MLWClient::value_updated(M2MBase *base, M2MBase::BaseType type) {
    printf("\nValue updated of Object name %s and Type %d\n",
           base->name().c_str(), type);
}
