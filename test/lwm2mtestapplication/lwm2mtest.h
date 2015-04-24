#include <unistd.h>
#include "lwm2m-client/m2minterfacefactory.h"
#include "lwm2m-client/m2minterface.h"
#include "lwm2m-client/m2mdevice.h"
#include "lwm2m-client/m2minterfaceobserver.h"
#include "lwm2m-client/m2mobjectinstance.h"
#include "lwm2m-client/m2mresource.h"

class M2MLWClient: public M2MInterfaceObserver {
public:
    M2MLWClient();

    ~M2MLWClient();

    bool create_interface(const char *endpoint,
                          const char *resource_type,
                          const int32_t lifetime,
                          const uint16_t listen_port,
                          const char *domain,
                          const uint8_t binding_mode,
                          const uint8_t network_interface);

    bool create_bootstrap_object(const char *coap_bootstrap_address);

    bool create_register_object(const char *coap_register_address);

    bool test_bootstrap();

    bool create_device_object(M2MDevice::DeviceResource resource,
                              const char *value);

    bool create_device_object(M2MDevice::DeviceResource resource,
                              uint32_t value);

    bool create_object(const char *name,bool new_instance);

    bool create_static_resource_string(const char *name,
                                       const char *value,
                                       bool multiple_instance,
                                       uint16_t object_instance = 0);

    bool create_static_resource_int(const char *name,
                                    uint32_t value,
                                    bool multiple_instance,
                                    uint16_t object_instance = 0);

    bool create_dynamic_resource(const char *name,
                                 bool observable,
                                 bool multiple_instance,
                                 uint16_t object_instance = 0);

    bool create_object_instance();

    bool test_register();

    bool test_update_register(const uint32_t lifetime);

    bool test_unregister();

    void bootstrap_done(M2MSecurity *server_object);

    void object_registered(M2MSecurity *security_object, const M2MServer &server_object);

    void object_unregistered(M2MSecurity *server_object);

    void registration_updated(M2MSecurity *security_object, const M2MServer &server_object);

    void error(M2MInterface::Error error);

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
