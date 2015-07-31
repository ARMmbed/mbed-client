/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include <unistd.h>
#include <malloc.h>
#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
#include <signal.h> /* For SIGIGN and SIGINT */
#include "lwm2m-client/m2minterfacefactory.h"
#include "lwm2m-client/m2mdevice.h"
#include "lwm2m-client/m2minterfaceobserver.h"
#include "lwm2m-client/m2minterface.h"
#include "lwm2m-client/m2mobjectinstance.h"
#include "lwm2m-client/m2mresource.h"

#include "ns_trace.h"

const String &BOOTSTRAP_SERVER_ADDRESS = "coap://10.45.3.10:5693";
const String &M2M_SERVER_ADDRESS = "coap://10.45.3.10:5683";
const String &MANUFACTURER = "manufacturer";
const String &TYPE = "type";
const String &MODEL_NUMBER = "2015";
const String &SERIAL_NUMBER = "12345";

const uint8_t STATIC_VALUE[] = "Open Mobile Alliance";

static void ctrl_c_handle_function(void);
void close_function();
static void display_mallinfo(void);
typedef void (*signalhandler_t)(int); /* Function pointer type for ctrl-c */

class M2MLWClient: public M2MInterfaceObserver {
public:
    M2MLWClient(){
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

    ~M2MLWClient() {
        if(_security) {
            delete _security;
        }
        if(_register_security){
            delete _register_security;
        }
        if(_device) {
           M2MDevice::delete_instance();
            _device = NULL;
        }
        if(_object) {
            delete _object;
        }
        if(_interface) {
            delete _interface;
        }
    }

    bool create_interface() {
        _interface = M2MInterfaceFactory::create_interface(*this,
                                                  "linux-endpoint",
                                                  "test",
                                                  60,
                                                  5683,
                                                  "",
                                                  M2MInterface::UDP,
                                                  M2MInterface::LwIP_IPv4,
                                                  "");
        printf("Endpoint Name : linux-endpoint\n");
        return (_interface == NULL) ? false : true;
    }

    bool bootstrap_successful() {
        while(!_bootstrapped && !_error) {
            sleep(1);
        }
        return _bootstrapped;
    }

    bool register_successful() {
        while(!_registered && !_error) {
            sleep(1);
        }
        return _registered;
    }

    bool unregister_successful() {
        while(!_unregistered && !_error) {
            sleep(1);
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
            _security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::NoSecurity)) {
                success = true;
                /* Not used now because there is no TLS or DTLS implementation available for stack.
                security->set_resource_value(M2MSecurity::ServerPublicKey,certificates->certificate_ptr[0],certificates->certificate_len[0]);
                security->set_resource_value(M2MSecurity::PublicKey,certificates->certificate_ptr[1],certificates->certificate_len[1]);
                security->set_resource_value(M2MSecurity::Secretkey,certificates->own_private_key_ptr,certificates->own_private_key_len);
                */
            }
        }
        printf("Bootstrap Server Address %s\n", BOOTSTRAP_SERVER_ADDRESS.c_str());
        return success;
    }

    bool create_register_object() {
        bool success = false;
        _register_security = M2MInterfaceFactory::create_security(M2MSecurity::M2MServer);
        if(_register_security) {
            if(_register_security->set_resource_value(M2MSecurity::M2MServerUri, M2M_SERVER_ADDRESS) &&            
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
            _device->object_instance()->set_operation(M2MBase::GET_PUT_POST_ALLOWED);
            if(_device->create_resource(M2MDevice::Manufacturer,MANUFACTURER)     &&
               _device->create_resource(M2MDevice::DeviceType,TYPE)        &&
               _device->create_resource(M2MDevice::ModelNumber,MODEL_NUMBER)      &&
               _device->create_resource(M2MDevice::SerialNumber,SERIAL_NUMBER)) {
                success = true;
            }
        }
        return success;
    }

    void execute_function(void *argument) {
        if(argument) {
            char* arguments = (char*)argument;
            printf("Received %s!!\n", arguments);
        }
        printf("I am executed !!\n");
    }

    bool create_generic_object() {
        bool success = false;
        _object = M2MInterfaceFactory::create_object("Test");
        if(_object) {
            _object->set_operation(M2MBase::GET_PUT_POST_ALLOWED);
            M2MObjectInstance* inst = _object->create_object_instance();
            if(inst) {
               inst->set_operation(M2MBase::GET_PUT_POST_ALLOWED);
               inst->set_observable(false);
               M2MResource* res = inst->create_dynamic_resource("1","ResourceTest",
                                                                 M2MResourceInstance::INTEGER,
                                                                 true,true);
                char buffer[20];
                int size = sprintf(buffer,"%d",_value);
                res->set_operation(M2MBase::GET_PUT_POST_ALLOWED);
                res->set_value((const uint8_t*)buffer,
                             (const uint32_t)size);
                res->set_execute_function(execute_callback(this,&M2MLWClient::execute_function));
                _value++;

                inst->create_static_resource("0",
                                             "ResourceTest",
                                             M2MResourceInstance::INTEGER,
                                             STATIC_VALUE,
                                             sizeof(STATIC_VALUE)-1);

                M2MResourceInstance* instance = inst->create_dynamic_resource_instance("1",
                                                                         "ResourceTest",
                                                                         M2MResourceInstance::INTEGER,
                                                                         true,0);

                if(instance) {
                    instance->set_operation(M2MBase::GET_PUT_ALLOWED);
                    instance->set_value((const uint8_t*)buffer,
                                 (const uint32_t)size);
                }
            }
        }
        return success;
    }

    void update_resource() {
        if(_object) {
            M2MObjectInstance* inst = _object->object_instance();
            if(inst) {
                M2MResource* res = inst->resource("1");
                if(res) {
                    printf(" Value sent %d\n", _value);
                    char buffer[20];
                    int size = sprintf(buffer,"%d",_value);
                    res->set_value((const uint8_t*)buffer,
                                   (const uint32_t)size);
                    _value++;
                }
                res = inst->resource("1");
                if(res) {
                    M2MResourceInstance *res_inst = res->resource_instance(0);
                    if(res_inst) {
                        char buffer1[20];
                        int size1 = sprintf(buffer1,"%d",_value);
                        res_inst->set_value((const uint8_t*)buffer1,
                                       (const uint32_t)size1);
                        _value++;
                    }
                }
            }
        }
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
            printf("mDS Address %s\n",
            _register_security->resource_value_string(M2MSecurity::M2MServerUri).c_str());
        }
    }

    void object_registered(M2MSecurity */*security_object*/, const M2MServer &/*server_object*/){
        _registered = true;
        printf("\nRegistered\n");
        printf("\n============== After Registering ==============\n");
        display_mallinfo();
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

    void value_updated(M2MBase *base, M2MBase::BaseType type) {
        printf("\nValue updated of Object name %s and Type %d\n",
               base->name().c_str(), type);
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
    int                 _value;
};

void* wait_for_bootstrap(void* arg) {
    M2MLWClient *client;
    client = (M2MLWClient*) arg;
    if(client->bootstrap_successful()) {
        printf("\n============== After Bootstrapping ==============\n");
        display_mallinfo();
        printf("Registering endpoint\n");
        client->test_register();
    }
    return NULL;
}

void* wait_for_unregister(void* arg) {
    M2MLWClient *client;
    client = (M2MLWClient*) arg;
    if(client->unregister_successful()) {
        printf("Unregistered done --> exiting\n");
        close_function();
    }
    return NULL;
}

void* send_observation(void* arg) {
    M2MLWClient *client;
    client = (M2MLWClient*) arg;
    static uint8_t counter = 0;
    while(1) {
        sleep(1);
        if(counter >= 4 &&
           client->register_successful()) {
            printf("Sending observation\n");
            client->update_resource();
            counter = 0;
            printf("\n============== After Sending Observation ==============\n");
            display_mallinfo();
        }
        else
            counter++;
    }
    return NULL;
}

static M2MLWClient *m2mclient = NULL;

static void ctrl_c_handle_function(void)
{
    if(m2mclient && m2mclient->register_successful()) {
        printf("Unregistering endpoint\n");
        m2mclient->test_unregister();
    }
}

void trace_printer(const char* str)
{
  printf("%s\r\n", str);
}

static pthread_t bootstrap_thread;
static pthread_t unregister_thread;
static pthread_t observation_thread;

void close_function() {
    pthread_cancel(bootstrap_thread);
    pthread_cancel(unregister_thread);
    pthread_cancel(observation_thread);
}

static void display_mallinfo(void)
{
    struct mallinfo mi;

   mi = mallinfo();

    printf("Total non-mmapped bytes (arena):       %d\n", mi.arena);
    printf("# of free chunks (ordblks):            %d\n", mi.ordblks);
    printf("# of free fastbin blocks (smblks):     %d\n", mi.smblks);
    printf("# of mapped regions (hblks):           %d\n", mi.hblks);
    printf("Bytes in mapped regions (hblkhd):      %d\n", mi.hblkhd);
    printf("Max. total allocated space (usmblks):  %d\n", mi.usmblks);
    printf("Free bytes held in fastbins (fsmblks): %d\n", mi.fsmblks);
    printf("Total allocated space (uordblks):      %d\n", mi.uordblks);
    printf("Total free space (fordblks):           %d\n", mi.fordblks);
    printf("Topmost releasable block (keepcost):   %d\n", mi.keepcost);
}

int main() {

    printf("============== Before allocating blocks ==============\n");
    display_mallinfo();

    M2MLWClient lwm2mclient;

    m2mclient = &lwm2mclient;

    trace_init();
    set_trace_print_function( trace_printer );
    set_trace_config(TRACE_MODE_COLOR|TRACE_ACTIVE_LEVEL_DEBUG|TRACE_CARRIAGE_RETURN);

    signal(SIGINT, (signalhandler_t)ctrl_c_handle_function);

    bool result = lwm2mclient.create_interface();
    if(true == result) {
        printf("\nInterface created\n");
    }
    result = lwm2mclient.create_bootstrap_object();
    if(true == result) {
        printf("Bootstrap object created");
    }

    result = lwm2mclient.create_register_object();
    if(true == result) {
        printf("Register object created");
    }

    result = lwm2mclient.create_device_object();
    if(true == result){
        printf("\nDevice object created !!\n");
    }

    result = lwm2mclient.create_generic_object();

    if(true == result) {
        printf("\nGeneric object created\n");
    }

    printf("Bootstrapping endpoint\n");
    lwm2mclient.test_bootstrap();

//    printf("Registering endpoint\n");
//    lwm2mclient.test_register();


    pthread_create(&bootstrap_thread, NULL, &wait_for_bootstrap, (void*) &lwm2mclient);
    pthread_create(&observation_thread, NULL, &send_observation, (void*) &lwm2mclient);
    pthread_create(&unregister_thread, NULL, &wait_for_unregister, (void*) &lwm2mclient);

    pthread_join(bootstrap_thread, NULL);
    pthread_join(unregister_thread, NULL);
    pthread_join(observation_thread, NULL);

    printf("\n============== After freeing blocks ==============\n");
    display_mallinfo();

    exit(EXIT_SUCCESS);
}

