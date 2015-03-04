#include "nsdlaccesshelper.h"
#include "m2mnsdlobserver.h"
#include "m2msecurity.h"
#include "m2mobject.h"
#include "m2mobjectinstance.h"
#include "m2mresource.h"
#include "m2mconstants.h"

M2MNsdlInterface::M2MNsdlInterface(M2MNsdlObserver &observer)
: _observer(observer),
  _nsdl_exceution_timer(new M2MTimer(*this)),
  _registration_timer(new M2MTimer(*this)),
  _counter_for_nsdl(0)
{
    _endpoint = NULL;
    _resource = NULL;    
    initialize();
}

M2MNsdlInterface::~M2MNsdlInterface()
{
    if(_resource) {
        if(_resource->resource_parameters_ptr) {
            memory_free(_resource->resource_parameters_ptr);
            _resource->resource_parameters_ptr = NULL;
        }
        memory_free(_resource);
        _resource = NULL;
    }
    if(_endpoint) {
        if(_endpoint->lifetime_ptr) {
            free(_endpoint->lifetime_ptr);
            _endpoint->lifetime_ptr = NULL;
        }
        memory_free(_endpoint);
        _endpoint = NULL;
    }
    delete _nsdl_exceution_timer;
    delete _registration_timer;
    _object_list.clear();
}

bool M2MNsdlInterface::initialize()
{
    __nsdl_interface = this;
    // Initialize the libNsdl    
    __sn_nsdl_memory.sn_nsdl_alloc = &(__nsdl_c_memory_alloc);
    __sn_nsdl_memory.sn_nsdl_free = &(__nsdl_c_memory_free);

    // This initializes libCoap and libNsdl
    // Parameters are function pointers to used memory allocation
    // and free functions in structure and used functions for sending
    // and receiving purposes.
    sn_nsdl_init(&(__nsdl_c_send_to_server), &(__nsdl_c_received_from_server), &(__sn_nsdl_memory));

    _nsdl_exceution_timer->start_timer(ONE_SECOND_TIMER * 1000,
                                       M2MTimerObserver::NsdlExecution,
                                       false);
    bool success = false;
    // If memory is already allocated
    // free it and allocate again.
    if(_resource) {
        if(_resource->resource_parameters_ptr) {
            memory_free(_resource->resource_parameters_ptr);
            _resource->resource_parameters_ptr = NULL;
        }
       memory_free(_resource);
       _resource = NULL;
    }
    // Allocate the memory for resources
    _resource = (sn_nsdl_resource_info_s*)memory_alloc(sizeof(sn_nsdl_resource_info_s));
    if(_resource) {
        memset(_resource, 0, sizeof(sn_nsdl_resource_info_s));
        _resource->resource_parameters_ptr = (sn_nsdl_resource_parameters_s*)memory_alloc(sizeof(sn_nsdl_resource_parameters_s));
        if(_resource->resource_parameters_ptr) {
            memset(_resource->resource_parameters_ptr, 0, sizeof(sn_nsdl_resource_parameters_s));
        } else {
            memory_free(_resource);
        }
    }

    // If memory is already allocated
    // free it and allocate again.
    if(_endpoint) {
       memory_free(_endpoint);
       _endpoint = NULL;
    }
    //Allocate the memory for endpoint
    _endpoint = (sn_nsdl_ep_parameters_s*)memory_alloc(sizeof(sn_nsdl_ep_parameters_s));
    if(_endpoint) {
        success = true;
    }
    return success;
}

void M2MNsdlInterface::create_endpoint(const String &name,
                                       const String &type,
                                       const uint32_t life_time,
                                       const String &domain,
                                       const uint8_t mode,
                                       const String &/*context_address*/)
{
    if(_endpoint){
        memset(_endpoint, 0, sizeof(sn_nsdl_ep_parameters_s));
        _endpoint->endpoint_name_ptr = (uint8_t*)name.c_str();
        _endpoint->endpoint_name_len = name.length();
        _endpoint->type_ptr = (uint8_t*)type.c_str();
        _endpoint->type_len =  type.length();
        _endpoint->domain_name_ptr = (uint8_t*)domain.c_str();
        _endpoint->domain_name_len = domain.length();
        _endpoint->binding_and_mode = (sn_nsdl_oma_binding_and_mode_t)mode;

        char buffer[20];
        int size = sprintf(buffer,"%ld",life_time);
        _endpoint->lifetime_ptr = (uint8_t*)malloc(sizeof(size));
        memcpy(_endpoint->lifetime_ptr,buffer,size);
        _endpoint->lifetime_len =  size;
    }
}

void M2MNsdlInterface::delete_endpoint()
{
    if(_endpoint) {
        if(_endpoint->lifetime_ptr) {
            free(_endpoint->lifetime_ptr);
            _endpoint->lifetime_ptr = NULL;
        }
        memory_free(_endpoint);
        _endpoint = NULL;
    }
}

bool M2MNsdlInterface::create_nsdl_list_structure(const M2MObjectList &object_list)
{
    bool success = false;
    if(!object_list.empty()) {
        M2MObjectList::const_iterator it;
        it = object_list.begin();
        for ( ; it != object_list.end(); it++ ) {
            // Create NSDL structure for all Objects inside
            success = create_nsdl_object_structure(*it);
            add_object_to_list(*it);
        }
    }
    return success;
}

bool M2MNsdlInterface::delete_nsdl_list_structure(const M2MObjectList &object_list)
{
    bool success = false;
    if(!object_list.empty()) {
        M2MObjectList::const_iterator it;
        it = object_list.begin();
        M2MObject* object = *it;
        for ( ; it != object_list.end(); it++ ) {
            success = delete_nsdl_object_structure(*object);
        }
    }
    return success;
}

bool M2MNsdlInterface::create_bootstrap_resource(sn_nsdl_addr_s *address)
{
    _bootstrap_device_setup.error_code = NO_ERROR;
    _bootstrap_device_setup.sn_oma_device_boot_callback = 0;

    _bootstrap_endpoint.device_object = &_bootstrap_device_setup;
    _bootstrap_endpoint.oma_bs_status_cb = &__nsdl_c_bootstrap_done;

    return ((sn_nsdl_oma_bootstrap(address, _endpoint, &_bootstrap_endpoint) == 0) ? true : false);
}

bool M2MNsdlInterface::send_register_message(uint8_t* address,
                                             const uint16_t port,
                                             sn_nsdl_addr_type_e address_type)
{
    return (set_NSP_address(address, port, address_type) == 0 &&
            sn_nsdl_register_endpoint(_endpoint) == 0) ? true : false;
}

bool M2MNsdlInterface::send_update_registration(const uint32_t lifetime)
{
    char buffer[20];
    int size = sprintf(buffer,"%ld",lifetime);
    //TODO: Currently there is no way to find out if the callback has come for
    // Update registration or not. Need API or implementation change on C side.
    return (sn_nsdl_update_registration((uint8_t*)buffer,
                                        (uint8_t)size) == 0) ? true : false;
}

bool M2MNsdlInterface::send_unregister_message()
{
    //Does not clean resources automatically
    return (sn_nsdl_unregister_endpoint() == 0) ? true : false;
}

void *M2MNsdlInterface::memory_alloc(uint16_t size)
{
    if(size)
        return malloc(size);
    else
        return 0;
}

void M2MNsdlInterface::memory_free(void *ptr)
{
    if(ptr)
        free(ptr);
}

uint8_t M2MNsdlInterface::send_to_server_callback(sn_nsdl_capab_e /*protocol*/,
                                                  uint8_t *data_ptr,
                                                  uint16_t data_len,
                                                  sn_nsdl_addr_s *address)
{
    _observer.coap_message_ready(data_ptr,data_len,address);
    return 1;
}

uint8_t M2MNsdlInterface::received_from_server_callback(sn_coap_hdr_s *coap_header,
                                                        sn_nsdl_addr_s */*address*/)
{
    _observer.coap_data_processed();
    uint8_t value = 0;
    if(coap_header) {

        /* If message is response to NSP registration */
        if((COAP_MSG_CODE_RESPONSE_CREATED == coap_header->msg_code)) {
             // Send callback about registration and unregistration
             // also send callback for all the send observation messages and
             // other messages received by the client from server.
             //TODO: Currently this is assumption that only callback coming here
             // is for registration however this same callback is also coming for
             // Update registration , need some mechanism to identify these two separately.
             _observer.client_registered();
             _registration_timer->start_timer(registration_time() * 1000,
                                              M2MTimerObserver::Registration,
                                              false);
        } else if(COAP_MSG_CODE_RESPONSE_DELETED == coap_header->msg_code) {
            _registration_timer->stop_timer();
            _observer.client_unregistered();
        }
    }
    return value;
}

uint8_t M2MNsdlInterface::resource_callback(sn_coap_hdr_s *received_coap_header,
                                            sn_nsdl_addr_s *address,
                                            sn_proto_info_s */*proto*/)
{
    _observer.coap_data_processed();
    uint8_t result = 1;

    String resource_name = coap_to_string(received_coap_header->uri_path_ptr,
                                          received_coap_header->uri_path_len);
    M2MBase* object = find_resource(resource_name);

    if(COAP_MSG_CODE_REQUEST_GET == received_coap_header->msg_code) {
        result = handle_get_request(received_coap_header,object,address);
    } else if(COAP_MSG_CODE_REQUEST_PUT == received_coap_header->msg_code) {
        result = handle_put_request(received_coap_header,object,address);
    }
    return result;
}

void M2MNsdlInterface::bootstrap_done_callback(sn_nsdl_oma_server_info_t *server_info)
{
    M2MSecurity* security = NULL;
    if(server_info && server_info->omalw_address_ptr->addr_ptr) {
        security = new M2MSecurity(M2MSecurity::M2MServer);
        uint16_t port = server_info->omalw_address_ptr->port;
        char buffer[10];
        sprintf(buffer,"%d",port);

        uint8_t* server_address = (uint8_t*) malloc(server_info->omalw_address_ptr->addr_len);
        memcpy(server_address, server_info->omalw_address_ptr->addr_ptr, server_info->omalw_address_ptr->addr_len);

        String server_uri(COAP);
        server_uri += String((char*)server_address);
        server_uri +=String(":");
        server_uri += String(buffer);

        if(server_address){
            free(server_address);
        }

        security->set_resource_value(M2MSecurity::M2MServerUri, server_uri);
        security->set_resource_value(M2MSecurity::BootstrapServer, 0);

        M2MSecurity::SecurityModeType security_mode = M2MSecurity::SecurityNotSet;

        switch(server_info->omalw_server_security) {
            case SEC_NOT_SET:
                security_mode = M2MSecurity::SecurityNotSet;
                break;
            case CERTIFICATE:
                security_mode = M2MSecurity::Certificate;
                break;
            case NO_SEC:
                security_mode = M2MSecurity::NoSecurity;
                break;
            default:
                break;
            }
        security->set_resource_value(M2MSecurity::SecurityMode,security_mode);

        //TODO: This is mandatory parameter for LWM2M server,
        // why is it missing from nsdl-c API ?
        security->set_resource_value(M2MSecurity::ShortServerID,1);

        // Check certiticates only if the mode is Certificate
        // else it is in NoSecurity Mode, Psk and Rsk are not supported.
        if(M2MSecurity::Certificate == security_mode) {
            omalw_certificate_list_t *certificates = sn_nsdl_get_certificates();
            if(certificates) {
                security->set_resource_value(M2MSecurity::ServerPublicKey,certificates->certificate_ptr[0],certificates->certificate_len[0]);
                security->set_resource_value(M2MSecurity::PublicKey,certificates->certificate_ptr[1],certificates->certificate_len[1]);
                security->set_resource_value(M2MSecurity::Secretkey,certificates->own_private_key_ptr,certificates->own_private_key_len);
            } else {
                // Mode is certificate but certificates are missing so its bootstrap error.
                delete security;
                security = NULL;
            }
        }
    }
    if(security) {
        // Inform that bootstrap is done and LWM2M server object is available.
        _observer.bootstrap_done(security);
    } else {
        // Bootstrap error inform to the application.
        _observer.bootstrap_error();
    }
}

bool M2MNsdlInterface::process_received_data(uint8_t *data,
                                             uint16_t data_size,
                                             sn_nsdl_addr_s *address)
{
    return (0 == sn_nsdl_process_coap(data, data_size, address)) ? true : false;
}

void M2MNsdlInterface::timer_expired(M2MTimerObserver::Type type)
{
    if(M2MTimerObserver::NsdlExecution == type) {
        sn_nsdl_exec(_counter_for_nsdl);
        _counter_for_nsdl++;
    } else if( M2MTimerObserver::Registration == type){
        sn_nsdl_update_registration(_endpoint->lifetime_ptr,
                                    _endpoint->lifetime_len);
    }
}

void M2MNsdlInterface::observation_to_be_sent(M2MBase *object)
{
    if(object) {
        uint8_t *value = 0;
        uint32_t length = 0;
        uint8_t observation_number = 0;

        object->get_value(value,length);
        observation_number = object->observation_number();

        sn_nsdl_send_observation_notification((uint8_t*)object->observation_token().c_str(),
                                              (uint8_t)object->observation_token().length(),
                                              value,length,
                                              &observation_number,
                                              sizeof(observation_number),
                                              COAP_MSG_TYPE_CONFIRMABLE,
                                              object->coap_content_type());
        memory_free(value);
    }
}

void M2MNsdlInterface::resource_to_be_deleted(const M2MBase &object)
{
    if(M2MBase::Object == object.base_type()){
        delete_nsdl_object_structure((const M2MObject&)object);
    } else if(M2MBase::ObjectInstance == object.base_type()) {
        delete_nsdl_object_instance_structure((const M2MObjectInstance&)object);
    } else if(M2MBase::Resource == object.base_type()){
        delete_nsdl_resource_structure((const M2MResource&)object);
    }
}

bool M2MNsdlInterface::create_nsdl_object_structure(const M2MObject *object)
{    
    bool success = false;
    if(object) {
        M2MObjectInstanceList instance_list = object->instances();
        if(!instance_list.empty()) {
           M2MObjectInstanceList::const_iterator it;
           it = instance_list.begin();
           for ( ; it != instance_list.end(); it++ ) {
               // Create NSDL structure for all object instances inside
               success = create_nsdl_object_instance_structure(*it);
           }
        }
    }    
    return success;
}

bool M2MNsdlInterface::create_nsdl_object_instance_structure(M2MObjectInstance *object_instance)
{
    bool success = false;
    if( object_instance) {
        char inst_id[10];
        sprintf(inst_id,"%d",object_instance->instance_id());

        // Append object instance id to the object name.
        String object_name = object_instance->name();
        object_name += String("/");
        object_name += String(inst_id);;

        if(_resource) {
            _resource->path = ((uint8_t*)memory_alloc(sizeof(object_name.length())));
            memset(_resource->path, 0, sizeof(uint8_t));
            if(_resource->path) {
                memcpy(_resource->path, (uint8_t*)object_name.c_str(), object_name.length());
                _resource->pathlen = (uint16_t)object_name.length();
            }
            _resource->access = (sn_grs_resource_acl_e)object_instance->operation();

            _resource->resource = 0;
            _resource->resourcelen = 0;

            if(_resource->resource_parameters_ptr) {

                if(!object_instance->resource_type().empty()) {
                    _resource->resource_parameters_ptr->resource_type_ptr =
                            ((uint8_t*)memory_alloc(sizeof(object_instance->resource_type().length())));
                    memcpy(_resource->path,
                           (uint8_t*)object_instance->resource_type().c_str(),
                           object_instance->resource_type().length());
                    _resource->resource_parameters_ptr->resource_type_len =
                            (uint16_t)object_instance->resource_type().length();
                }
                if(!object_instance->interface_description().empty()) {
                    _resource->resource_parameters_ptr->interface_description_ptr =
                            ((uint8_t*)memory_alloc(sizeof(object_instance->interface_description().length())));
                    memcpy(_resource->path,
                           (uint8_t*)object_instance->interface_description().c_str(),
                           object_instance->resource_type().length());
                    _resource->resource_parameters_ptr->interface_description_len =
                            (uint16_t)object_instance->interface_description().length();
                }

                _resource->resource_parameters_ptr->coap_content_type = object_instance->coap_content_type();
                _resource->resource_parameters_ptr->observable = (uint8_t)object_instance->is_observable();
            }

            M2MResourceList res_list = object_instance->resources();
            if(!res_list.empty()) {
                M2MResourceList::const_iterator it;
                it = res_list.begin();
                for ( ; it != res_list.end(); it++ ) {
                    // Create NSDL structure for all resources inside
                    success = create_nsdl_resource_structure(*it,object_name);
                }
            } else {
                (sn_nsdl_create_resource(_resource) == 0) ? success = true : success = false;
                if(success) {
                    object_instance->set_under_observation(false,this);
                }
            }
        }
    }
    return success;
}

bool M2MNsdlInterface::create_nsdl_resource_structure(M2MResource *res,
                                                      const String &object_name)
{
    bool success = false;
    // Create the NSDL Resource Pointer...
    if(res) {
        if(_resource) {
            //TODO: implement access control
            // Currently complete access is given
           _resource->access = (sn_grs_resource_acl_e)res->operation();

           uint8_t* buffer = 0;
           uint32_t length = 0;

           M2MResource::Mode mode = res->mode();

           if(M2MResource::Static == mode) {
               // Static resource is updated
               _resource->mode = SN_GRS_STATIC;
               M2MResource *res_ptr = const_cast<M2MResource*>(res);
               res_ptr->get_value(buffer,length);
           } else if(M2MResource::Dynamic ){
              // Dynamic resource is updated
               _resource->mode = SN_GRS_DYNAMIC;
               //_resource->sn_grs_dyn_res_callback = res->callback;
           } else {
               _resource->mode = SN_GRS_DIRECTORY;
           }

           _resource->resource = (uint8_t *)buffer;
           _resource->resourcelen = length;

           if(buffer) {
               free(buffer);
           }

           // Append object name to the resource.
           // Take out the instance Id and append to the
           // resource name like "object/0/+ resource + / + 0"
           char inst_id[10];
           sprintf(inst_id,"%d",res->instance_id());

           String res_name = object_name;
           res_name+= "/";
           res_name+= inst_id;

           _resource->path = ((uint8_t*)memory_alloc(sizeof(res_name.length())));
           memset(_resource->path, 0, sizeof(uint8_t));
           if(_resource->path) {
               memcpy(_resource->path, (uint8_t*)res_name.c_str(), res_name.length());
               _resource->pathlen = res_name.length();
           }

           if(_resource->resource_parameters_ptr) {

               if(!res->resource_type().empty()) {
                   _resource->resource_parameters_ptr->resource_type_ptr =
                           ((uint8_t*)memory_alloc(sizeof(res->resource_type().length())));
                   memcpy(_resource->path,
                          (uint8_t*)res->resource_type().c_str(),
                          res->resource_type().length());
                   _resource->resource_parameters_ptr->resource_type_len =
                           res->resource_type().length();
               }
               if(!res->interface_description().empty()) {
                   _resource->resource_parameters_ptr->interface_description_ptr =
                           ((uint8_t*)memory_alloc(sizeof(res->interface_description().length())));
                   memcpy(_resource->path,
                          (uint8_t*)res->interface_description().c_str(),
                          res->resource_type().length());
                   _resource->resource_parameters_ptr->interface_description_len =
                           res->interface_description().length();
               }

               _resource->resource_parameters_ptr->coap_content_type = res->coap_content_type();
               _resource->resource_parameters_ptr->observable = (uint8_t)res->is_observable();
           }

           (sn_nsdl_create_resource(_resource) == 0) ? success = true : success = false;
           if(success) {
               res->set_under_observation(false,this);
           }
        }

    }
    return success;
}

bool M2MNsdlInterface::delete_nsdl_object_structure(const M2MObject &object)
{
    bool success = false;
    M2MObjectInstanceList instance_list = object.instances();
    if(!instance_list.empty()) {
        M2MObjectInstanceList::const_iterator it;
        it = instance_list.begin();
        M2MObjectInstance* instance = *it;
        for ( ; it != instance_list.end(); it++ ) {
            success = delete_nsdl_object_instance_structure(*instance);
        }
    }
    const M2MObject *obj = &object;
    remove_object_from_list(obj);
    return success;
}

bool M2MNsdlInterface::delete_nsdl_object_instance_structure(const M2MObjectInstance &object_instance)
{
    int8_t delete_result = -1;

    String object_name = "";
    if(!object_instance.name().empty()) {

        // Take out the instance Id and append to the
        // object name like "object + / + 0"
        char inst_id[10];
        sprintf(inst_id,"%d",object_instance.instance_id());

        object_name = object_instance.name();
        object_name+= "/";
        object_name+= inst_id;
        delete_result = sn_nsdl_delete_resource(object_name.length(), (uint8_t *)object_name.c_str());
    } else {
       // Delete all resources without object name
       // Should this be even possible ?
       M2MResourceList res_list = object_instance.resources();
       if(!res_list.empty()) {
           M2MResourceList::const_iterator it;
           it = res_list.begin();
           String res_name = "";
           for ( ; it != res_list.end(); it++ ) {
               char inst_id[10];
               sprintf(inst_id,"%d",(*it)->instance_id());
               res_name = (*it)->name();
               res_name+= "/";
               res_name+= inst_id;
               delete_result = sn_nsdl_delete_resource(res_name.length(),(uint8_t *)res_name.c_str());
           }
       }
    }
    return ((0 == delete_result) ? true : false);
}

bool M2MNsdlInterface::delete_nsdl_resource_structure(const M2MResource &resource)
{
    int8_t delete_result = false;
    char inst_id[10];
    sprintf(inst_id,"%d",resource.instance_id());
    String res_name = resource.name();
    res_name+= "/";
    res_name+= inst_id;

    delete_result = sn_nsdl_delete_resource(res_name.length(),(uint8_t *)res_name.c_str());
    return ((0 == delete_result) ? true : false);
}

// convenience method to get the URI from its buffer field...
String M2MNsdlInterface::coap_to_string(uint8_t *coap_data,int coap_data_length)
{
    if (coap_data != NULL && coap_data_length > 0) {
        char buf[256+1];
        memset(buf,0,256+1);
        memcpy(buf,(char *)coap_data,coap_data_length);
        return String(buf);
    }
    return String("");
}

uint64_t M2MNsdlInterface::registration_time()
{
    uint64_t value = 0;
    if(_endpoint->lifetime_len) {
        value = (uint64_t)atoi((const char*)_endpoint->lifetime_ptr);
    }
    // If the registration life time is set more than
    // 60 seconds then set re-registration timer to
    // be 80% of the lifetime value.
    if( value >= MINIMUM_REGISTRATION_TIME ){
        value = 0.8f * value;
    } else {
        value = MINIMUM_REGISTRATION_TIME;
    }
    return value;
}

M2MBase* M2MNsdlInterface::find_resource(const String &object_name)
{
    M2MBase *object = NULL;
    if(!_object_list.empty()) {
        M2MObjectList::const_iterator it;
        it = _object_list.begin();
        for ( ; it != _object_list.end(); it++ ) {
            object = find_resource((*it),object_name);
            if(object){
                break;
            }
        }
    }
    return object;
}

M2MBase* M2MNsdlInterface::find_resource(const M2MObject *object,
                                         const String &object_instance)
{
    M2MBase *instance = NULL;
    if(object) {
        M2MObjectInstanceList list = object->instances();
        if(!list.empty()) {
            M2MObjectInstanceList::const_iterator it;
            it = list.begin();
            for ( ; it != list.end(); it++ ) {
                char inst_id[10];
                sprintf(inst_id,"%d",(*it)->instance_id());

                // Append object instance id to the object name.
                String name = (*it)->name();
                name+= "/";
                name+= inst_id;
                if(name.compare(0, object_instance.size(), object_instance) ==0){
                    instance = (*it);
                    break;
                }
                instance = find_resource((*it),object_instance);
                if(instance){
                    break;
                }
            }
        }
    }
    return instance;
}

M2MBase* M2MNsdlInterface::find_resource(const M2MObjectInstance *object_instance,
                                         const String &resource_instance)
{
    M2MBase *resource = NULL;
    if(object_instance) {

        String name = object_instance->name();
        char obj_inst_id[10];
        sprintf(obj_inst_id,"%d",object_instance->instance_id());

        // Append object instance id to the object name.
        name+= "/";
        name+= obj_inst_id;

        M2MResourceList list = object_instance->resources();
        if(!list.empty()) {
            M2MResourceList::const_iterator it;
            it = list.begin();
            for ( ; it != list.end(); it++ ) {
                char inst_id[10];
                sprintf(inst_id,"%d",(*it)->instance_id());

                // Append object instance id to the object name.
                name+= (*it)->name();
                name+= "/";
                name+= inst_id;
                if(name.compare(0, resource_instance.size(),resource_instance) ==0){
                    resource = (*it);
                    break;
                }
            }
        }
    }
    return resource;
}

bool M2MNsdlInterface::object_present(M2MObject* object) const
{
    bool success = false;
    if(object && !_object_list.empty()) {
        M2MObjectList::const_iterator it;
        it = _object_list.begin();
        for ( ; it != _object_list.end(); it++ ) {
            if((*it) == object) {
                success = true;
                break;
            }
        }
    }
    return success;
}

bool M2MNsdlInterface::add_object_to_list(M2MObject* object)
{
    bool success = false;
    if(object && !object_present(object)) {
        _object_list.push_back(object);
        success = true;
    }
    return success;
}

bool M2MNsdlInterface::remove_object_from_list(const M2MObject *object)
{
    bool success = false;
    if(object && !_object_list.empty()) {
        M2MObjectList::const_iterator it;
        it = _object_list.begin();
        int index = 0;
        for ( ; it != _object_list.end(); it++, index++ ) {
            if((*it) == object) {
                _object_list.erase(index);
                success = true;
                break;
            }
        }
    }
    if(_object_list.empty()) {
        _object_list.clear();
    }
    return success;
}


uint8_t M2MNsdlInterface::handle_get_request(sn_coap_hdr_s *received_coap_header,
                                          M2MBase *object,
                                          sn_nsdl_addr_s *address)
{
    uint8_t result = 1;
    sn_coap_hdr_s * coap_response = NULL;
    // process the GET if we have registered a callback for it
    if ((object->operation() & SN_GRS_GET_ALLOWED) != 0) {
        coap_response = sn_coap_build_response(received_coap_header, COAP_MSG_CODE_RESPONSE_CONTENT);
        if(coap_response) {
            char content_type[10];
            int content_type_size = sprintf(content_type,"%d",object->coap_content_type());

            coap_response->content_type_ptr = (uint8_t*)memory_alloc(content_type_size);
            if(coap_response->content_type_ptr) {
                memcpy(coap_response->content_type_ptr,content_type,content_type_size);
                coap_response->content_type_len = (uint8_t)content_type_size;
            }

            // call the resource get() to get value
            uint8_t *value = 0;
            uint32_t length = 0;
            object->get_value(value,length);

            // fill in the CoAP response payload
            coap_response->payload_len = length;
            coap_response->payload_ptr = value;

            coap_response->options_list_ptr = (sn_coap_options_list_s*)memory_alloc(sizeof(sn_coap_options_list_s));
            memset(coap_response->options_list_ptr, 0, sizeof(sn_coap_options_list_s));

            if(received_coap_header->token_ptr) {
                char *token_buffer = (char*)malloc(received_coap_header->token_len);
                memcpy(token_buffer,
                       (char*)received_coap_header->token_ptr,
                       received_coap_header->token_len);

                String token(token_buffer);

                object->set_observation_token(token);
                if(token_buffer) {
                    free(token_buffer);
                }
            }
            if(received_coap_header->options_list_ptr &&
               received_coap_header->options_list_ptr->observe) {
                uint8_t observe_option;
                observe_option = *received_coap_header->options_list_ptr->observe_ptr;
                if(START_OBSERVATION == observe_option) {
                    object->set_under_observation(true,this);
                    uint8_t observation_number = object->observation_number();
                    coap_response->options_list_ptr->observe_ptr = &observation_number;
                    coap_response->options_list_ptr->observe_len = sizeof(observation_number);
                } else if (STOP_OBSERVATION == observe_option) {
                    object->set_under_observation(false,NULL);
                }
            }
        }
    }else {
        // Operation is not allowed.
        coap_response = sn_coap_build_response(received_coap_header, COAP_MSG_CODE_RESPONSE_BAD_REQUEST);
        coap_response->options_list_ptr = 0;
        coap_response->content_type_ptr = 0;
     }
    if(coap_response) {
        // build response and send
        (sn_nsdl_send_coap_message(address, coap_response) == 0) ? result = 0 : result = 1;
        memory_free(coap_response->options_list_ptr);
        memory_free(coap_response->content_type_ptr);

        sn_coap_parser_release_allocated_coap_msg_mem(coap_response);
    }
    return result;
}

uint8_t M2MNsdlInterface::handle_put_request(sn_coap_hdr_s *received_coap_header,
                                          M2MBase *object,
                                          sn_nsdl_addr_s *address)
{
    uint8_t result = 1;
    sn_coap_hdr_s * coap_response = NULL;
    // process the PUT if we have registered a callback for it
    if(object) {
        if ((object->operation() & SN_GRS_PUT_ALLOWED) != 0) {        
            object->set_value(received_coap_header->payload_ptr, received_coap_header->payload_len);

            if(received_coap_header->options_list_ptr->uri_query_ptr) {
                char *query = (char*)memory_alloc(received_coap_header->options_list_ptr->uri_query_len+1);
                if (query){
                    memcpy(query,
                        received_coap_header->options_list_ptr->uri_query_ptr,
                        received_coap_header->options_list_ptr->uri_query_len);
                    memset(query + received_coap_header->options_list_ptr->uri_query_len,'\0',1);//String terminator
                    // if anything was updated, re-initialize the stored notification attributes
                    if (object->parse_notification_attribute(query)){
                        coap_response = sn_coap_build_response(received_coap_header, COAP_MSG_CODE_RESPONSE_CHANGED); // 2.04
                    } else {
                        // if query options are sent but no notification attribute names were found, it's an error
                        coap_response = sn_coap_build_response(received_coap_header, COAP_MSG_CODE_RESPONSE_BAD_REQUEST);// 4.00
                    }
                    memory_free(query);
                }
            }
            if(coap_response) {
                (sn_nsdl_send_coap_message(address, coap_response) == 0) ? result = 0 : result = 1;
                sn_coap_parser_release_allocated_coap_msg_mem(coap_response);
            }
        }
    }
    return result;
}

uint8_t M2MNsdlInterface::handle_post_request(sn_coap_hdr_s */*received_coap_header*/,
                                              M2MBase */*object*/,
                                              sn_nsdl_addr_s */*address*/)
{
    //TODO: Post request callback are currently missing in C side as well
    // Need new JIRA ticket for implementation on C side.
    uint8_t result = 1;
    return result;
}
