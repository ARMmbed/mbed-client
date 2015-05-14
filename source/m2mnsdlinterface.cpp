/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "include/nsdlaccesshelper.h"
#include "include/m2mnsdlobserver.h"
#include "lwm2m-client/m2msecurity.h"
#include "lwm2m-client/m2mserver.h"
#include "lwm2m-client/m2mobject.h"
#include "lwm2m-client/m2mobjectinstance.h"
#include "lwm2m-client/m2mresource.h"
#include "lwm2m-client/m2mconstants.h"

M2MNsdlInterface::M2MNsdlInterface(M2MNsdlObserver &observer)
: _observer(observer),
  _server(NULL),
  _nsdl_exceution_timer(new M2MTimer(*this)),
  _registration_timer(new M2MTimer(*this)),
  _nsdl_handle(NULL),
  _counter_for_nsdl(0),
  _register_id(0),
  _unregister_id(0),
  _update_id(0)
{
    _endpoint = NULL;
    _resource = NULL;
    __nsdl_interface = this;

    // This initializes libCoap and libNsdl
    // Parameters are function pointers to used memory allocation
    // and free functions in structure and used functions for sending
    // and receiving purposes.
    _nsdl_handle = sn_nsdl_init(&(__nsdl_c_send_to_server), &(__nsdl_c_received_from_server),
                 &(__nsdl_c_memory_alloc), &(__nsdl_c_memory_free));

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

    if(_server){
        delete _server;
        _server = NULL;
    }
    sn_nsdl_destroy(_nsdl_handle);
    _nsdl_handle = NULL;
    __nsdl_interface = NULL;
}

bool M2MNsdlInterface::initialize()
{
    bool success = false;

    _nsdl_exceution_timer->start_timer(ONE_SECOND_TIMER * 1000,
                                       M2MTimerObserver::NsdlExecution,
                                       false);

    // Allocate the memory for resources
    _resource = (sn_nsdl_resource_info_s*)memory_alloc(sizeof(sn_nsdl_resource_info_s));
    if(_resource) {
        memset(_resource, 0, sizeof(sn_nsdl_resource_info_s));
        _resource->resource_parameters_ptr = (sn_nsdl_resource_parameters_s*)memory_alloc(sizeof(sn_nsdl_resource_parameters_s));
        if(_resource->resource_parameters_ptr) {
            memset(_resource->resource_parameters_ptr, 0, sizeof(sn_nsdl_resource_parameters_s));
        }
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
                                       const int32_t life_time,
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
        if(!domain.empty()) {
            _endpoint->domain_name_ptr = (uint8_t*)domain.c_str();
            _endpoint->domain_name_len = domain.length();
        }
        _endpoint->binding_and_mode = (sn_nsdl_oma_binding_and_mode_t)mode;

        // If lifetime is less than zero then leave the field empty
        if( life_time > 0) {
            char buffer[20];
            int size = sprintf(buffer,"%ld",life_time);
            _endpoint->lifetime_ptr = (uint8_t*)memory_alloc(size);
            if(_endpoint->lifetime_ptr) {
                memcpy(_endpoint->lifetime_ptr,buffer,size);
                _endpoint->lifetime_len =  size;
            }
        }
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

bool M2MNsdlInterface::delete_nsdl_resource(const String &resource_name)
{
    return (sn_nsdl_delete_resource(_nsdl_handle,
                                    resource_name.length(),
                                    (uint8_t *)resource_name.c_str()) == 0) ? true : false;
}

bool M2MNsdlInterface::create_bootstrap_resource(sn_nsdl_addr_s *address)
{
    _bootstrap_device_setup.error_code = NO_ERROR;
    _bootstrap_device_setup.sn_oma_device_boot_callback = 0;

    _bootstrap_endpoint.device_object = &_bootstrap_device_setup;
    _bootstrap_endpoint.oma_bs_status_cb = &__nsdl_c_bootstrap_done;

    return ((sn_nsdl_oma_bootstrap(_nsdl_handle,
                                   address,
                                   _endpoint,
                                   &_bootstrap_endpoint) == 0) ? true : false);
}

bool M2MNsdlInterface::send_register_message(uint8_t* address,
                                             const uint16_t port,
                                             sn_nsdl_addr_type_e address_type)
{
    bool success = false;
    if(set_NSP_address(_nsdl_handle,address, port, address_type) == 0) {
        if(_register_id == 0) {
            success = true;
            _register_id = sn_nsdl_register_endpoint(_nsdl_handle,_endpoint);
        }
    }
    return success;
}

bool M2MNsdlInterface::send_update_registration(const uint32_t lifetime)
{
    bool success = false;
    char buffer[20];
    int size = sprintf(buffer,"%ld",lifetime);
    //TODO: Currently there is no way to find out if the callback has come for
    // Update registration or not. Need API or implementation change on C side.    
    if(_update_id == 0) {
        success = true;
        _update_id = sn_nsdl_update_registration(_nsdl_handle,
                                                 (uint8_t*)buffer,
                                                 (uint8_t)size);
    }
    return success;
}

bool M2MNsdlInterface::send_unregister_message()
{
    bool success = false;
    //Does not clean resources automatically
    if(_unregister_id == 0) {
       success = true;
        _unregister_id = sn_nsdl_unregister_endpoint(_nsdl_handle);
    }
    return success;
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

uint8_t M2MNsdlInterface::send_to_server_callback(struct nsdl_s * /*nsdl_handle*/,
                                                  sn_nsdl_capab_e /*protocol*/,
                                                  uint8_t *data_ptr,
                                                  uint16_t data_len,
                                                  sn_nsdl_addr_s *address)
{
    _observer.coap_message_ready(data_ptr,data_len,address);
    return 1;
}

uint8_t M2MNsdlInterface::received_from_server_callback(struct nsdl_s * /*nsdl_handle*/,
                                                        sn_coap_hdr_s *coap_header,
                                                        sn_nsdl_addr_s */*address*/)
{
    _observer.coap_data_processed();
    uint8_t value = 0;
    if(coap_header) {
        if(coap_header->msg_id == _register_id) {
            _register_id = 0;
            if(coap_header->msg_code == COAP_MSG_CODE_RESPONSE_CREATED) {
                if(_server) {
                    delete _server;
                    _server = NULL;
                }
                _server = new M2MServer();
                _server->set_resource_value(M2MServer::ShortServerID,1);

                _observer.client_registered(_server);
                if(_endpoint->lifetime_ptr) {
                    _registration_timer->start_timer(registration_time() * 1000,
                                                     M2MTimerObserver::Registration,
                                                     false);
                }
            } else {
                if(_server) {
                    delete _server;
                    _server = NULL;
                }
                _observer.registration_error(coap_header->msg_code);
            }
        } else if(coap_header->msg_id == _unregister_id) {
            _unregister_id = 0;
            if(coap_header->msg_code == COAP_MSG_CODE_RESPONSE_DELETED) {
                _registration_timer->stop_timer();
                if(_server) {
                   delete _server;
                   _server = NULL;
                }
                _observer.client_unregistered();
            } else {
                _observer.registration_error(coap_header->msg_code);
            }
        } else if(coap_header->msg_id == _update_id) {
            _update_id = 0;
            _observer.registration_updated(*_server);
        }
    }
    return value;
}

uint8_t M2MNsdlInterface::resource_callback(struct nsdl_s */*nsdl_handle*/,
                                            sn_coap_hdr_s *received_coap_header,
                                            sn_nsdl_addr_s *address,
                                            sn_nsdl_capab_e /*nsdl_capab*/)
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
    } else if(COAP_MSG_CODE_REQUEST_POST == received_coap_header->msg_code) {
        result = handle_post_request(received_coap_header,object,address);
    }
    return result;
}

void M2MNsdlInterface::bootstrap_done_callback(sn_nsdl_oma_server_info_t *server_info)
{
    M2MSecurity* security = NULL;
    if(server_info && server_info->omalw_address_ptr->addr_ptr &&
       (SN_NSDL_ADDRESS_TYPE_IPV4 == server_info->omalw_address_ptr->type)) {
        security = new M2MSecurity(M2MSecurity::M2MServer);
        uint16_t port = server_info->omalw_address_ptr->port;
        char buffer[10];
        sprintf(buffer,"%d",port);

        //TODO: currently only supports IPV4 Mapping, fix to support IPV6 as well

        String server_address;
        int val = 0;
        for(int index = 0; index < server_info->omalw_address_ptr->addr_len; index++) {
            char server_buffer[3];
            val = (int)server_info->omalw_address_ptr->addr_ptr[index];
            sprintf(server_buffer,"%d",val);
            server_address +=String(server_buffer);
            if(index < server_info->omalw_address_ptr->addr_len-1) {
                server_address += String(".");
            }
        }

        String server_uri(COAP);
        server_uri += server_address;
        server_uri +=String(":");
        server_uri += String(buffer);

        security->set_resource_value(M2MSecurity::M2MServerUri, server_uri);
        security->set_resource_value(M2MSecurity::BootstrapServer, 0);

        M2MSecurity::SecurityModeType security_mode = M2MSecurity::SecurityNotSet;

        switch(server_info->omalw_server_security) {
            case SEC_NOT_SET:
                security_mode = M2MSecurity::SecurityNotSet;
                break;
            case PSK:
                //Not supported at the moment
                break;
            case RPK:
                //Not supported at the moment
                break;
            case CERTIFICATE:
                security_mode = M2MSecurity::Certificate;
                break;
            case NO_SEC:
                security_mode = M2MSecurity::NoSecurity;
                break;
            }
        security->set_resource_value(M2MSecurity::SecurityMode,security_mode);

        //TODO: This is mandatory parameter for LWM2M server,
        // why is it missing from nsdl-c API ?
        security->set_resource_value(M2MSecurity::ShortServerID,1);

        // Check certiticates only if the mode is Certificate
        // else it is in NoSecurity Mode, Psk and Rsk are not supported.
        if(M2MSecurity::Certificate == security_mode) {
            omalw_certificate_list_t *certificates = sn_nsdl_get_certificates(_nsdl_handle);
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
    return (0 == sn_nsdl_process_coap(_nsdl_handle,
                                      data,
                                      data_size,
                                      address)) ? true : false;
}

void M2MNsdlInterface::timer_expired(M2MTimerObserver::Type type)
{
    if(M2MTimerObserver::NsdlExecution == type) {
        sn_nsdl_exec(_counter_for_nsdl);
        _counter_for_nsdl++;
    } else if(M2MTimerObserver::Registration == type) {
        if(_endpoint && _endpoint->lifetime_ptr) {
            sn_nsdl_update_registration(_nsdl_handle,
                                        _endpoint->lifetime_ptr,
                                        _endpoint->lifetime_len);
        }
    }
}

void M2MNsdlInterface::observation_to_be_sent(M2MBase *object)
{
    if(object) {
        uint8_t *value = 0;
        uint32_t length = 0;
        uint8_t *token = 0;
        uint32_t token_length = 0;
        uint8_t observation_number[2];
        uint8_t observation_number_length = 1;

        uint16_t number = object->observation_number();

        observation_number[0] = ((number>>8) & 0xFF);
        observation_number[1] = (number & 0xFF);

        if(number > 0xFF) {
            observation_number_length = 2;
        }

        object->get_value(value,length);
        object->get_observation_token(token,token_length);

        sn_nsdl_send_observation_notification(_nsdl_handle,
                                              token,
                                              token_length,
                                              value,length,
                                              observation_number,
                                              observation_number_length,
                                              COAP_MSG_TYPE_CONFIRMABLE,
                                              object->coap_content_type());
        memory_free(value);
        memory_free(token);
    }
}

void M2MNsdlInterface::resource_to_be_deleted(const String &resource_name)
{
    delete_nsdl_resource(resource_name);
}

void M2MNsdlInterface::remove_object(M2MBase *object)
{
    M2MObject* rem_object = (M2MObject*)object;
    if(rem_object && !_object_list.empty()) {
        M2MObjectList::const_iterator it;
        it = _object_list.begin();
        int index = 0;
        for ( ; it != _object_list.end(); it++, index++ ) {
            if((*it) == rem_object) {
                _object_list.erase(index);
                break;
            }
        }
    }
    if(_object_list.empty()) {
        _object_list.clear();
    }
}

bool M2MNsdlInterface::create_nsdl_object_structure(M2MObject *object)
{    
    bool success = false;
    if(object) {
        object->set_under_observation(false,this);
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
        object_name += String(inst_id);

        object_instance->set_under_observation(false,this);

        M2MResourceList res_list = object_instance->resources();
        if(!res_list.empty()) {
            M2MResourceList::const_iterator it;
            it = res_list.begin();
            for ( ; it != res_list.end(); it++ ) {
                // Create NSDL structure for all resources inside
                success = create_nsdl_resource_structure(*it,object_name);
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

           M2MBase::Mode mode = res->mode();

           if(M2MBase::Static == mode) {
                // Static resource is updated
                _resource->mode = SN_GRS_STATIC;
                res->get_value(buffer,length);
           } else if(M2MBase::Dynamic == mode){
              // Dynamic resource is updated
               _resource->mode = SN_GRS_DYNAMIC;
               _resource->sn_grs_dyn_res_callback = __nsdl_c_callback;
           } else {
               _resource->mode = SN_GRS_DIRECTORY;
           }

           _resource->resource = buffer;
           _resource->resourcelen = length;

           // Append object name to the resource.
           // Take out the instance Id and append to the
           // resource name like "object/0/+ resource + / + 0"
           String res_name = object_name;
           res_name+= String("/");
           res_name.append(res->name().c_str(),res->name().length());


           // if there are multiple instances supported
           // then add instance Id into creating resource path
           // else normal /object_id/object_instance/resource_id format.
           if(res->supports_multiple_instances()) {
               char inst_id[10];
               sprintf(inst_id,"%d",res->instance_id());
               res_name+= String("/") ;
               res_name+= String(inst_id);
            }

           _resource->path = ((uint8_t*)memory_alloc(res_name.length()));
           if(_resource->path) {
               memset(_resource->path, 0, res_name.length());
               memcpy(_resource->path, (uint8_t*)res_name.c_str(), res_name.length());
               _resource->pathlen = res_name.length();
           }

           if(!res->resource_type().empty() && _resource->resource_parameters_ptr) {
               _resource->resource_parameters_ptr->resource_type_ptr =
                       ((uint8_t*)memory_alloc(res->resource_type().length()));
               if(_resource->resource_parameters_ptr->resource_type_ptr) {
                   memcpy(_resource->resource_parameters_ptr->resource_type_ptr,
                          (uint8_t*)res->resource_type().c_str(),
                          res->resource_type().length());
                   _resource->resource_parameters_ptr->resource_type_len =
                           res->resource_type().length();
                }
           }
           if(!res->interface_description().empty() && _resource->resource_parameters_ptr) {
               _resource->resource_parameters_ptr->interface_description_ptr =
                       ((uint8_t*)memory_alloc(res->interface_description().length()));
               if(_resource->resource_parameters_ptr->interface_description_ptr) {
                   memcpy(_resource->resource_parameters_ptr->interface_description_ptr,
                          (uint8_t*)res->interface_description().c_str(),
                          res->interface_description().length());
                   _resource->resource_parameters_ptr->interface_description_len =
                           res->interface_description().length();
                }
           }
           if(_resource->resource_parameters_ptr) {
                _resource->resource_parameters_ptr->coap_content_type = res->coap_content_type();
                _resource->resource_parameters_ptr->observable = (uint8_t)res->is_observable();
           }

           int8_t result = sn_nsdl_create_resource(_nsdl_handle,_resource);
           // Either the resource is created or it already
           // exists , then result is success.
            if (result == 0 ||
               result == -2){
                success = true;
            }

            if(_resource->path) {
                memory_free(_resource->path);
            }
            if(_resource->resource_parameters_ptr->resource_type_ptr){
                memory_free(_resource->resource_parameters_ptr->resource_type_ptr);
            }
            if(_resource->resource_parameters_ptr->interface_description_ptr){
                memory_free(_resource->resource_parameters_ptr->interface_description_ptr);
            }

           //Clear up the filled resource to fill up new resource.
           clear_resource(_resource);

           if(buffer){
               memory_free(buffer);
           }
           if(success) {
               res->set_under_observation(false,this);
           }
        }
    }
    return success;
}

// convenience method to get the URI from its buffer field...
String M2MNsdlInterface::coap_to_string(uint8_t *coap_data,int coap_data_length)
{
    String value = "";
    if (coap_data != NULL && coap_data_length > 0) {
        char buf[256+1];
        memset(buf,0,256+1);
        memcpy(buf,(char *)coap_data,coap_data_length);
        value = String(buf);
    }
    return value;
}

uint64_t M2MNsdlInterface::registration_time()
{
    uint64_t value = 0;
    if(_endpoint->lifetime_ptr) {
        value = (uint64_t)atoi((const char*)_endpoint->lifetime_ptr);
    }

    if(value >= OPTIMUM_LIFETIME) {
        value = value - REDUCE_LIFETIME;
    } else {
        value = REDUCTION_FACTOR * value;
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
            if((*it)->name() == object_name) {
                object = (*it);
                break;
            }
            object = find_resource((*it),object_name);
            if(object != NULL) {
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
                name+= String("/");
                name+= String(inst_id);
                if(name == object_instance){
                    instance = (*it);
                    break;
                }
                instance = find_resource((*it),object_instance);
                if(instance != NULL){
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
        M2MResourceList list = object_instance->resources();
        if(!list.empty()) {
            M2MResourceList::const_iterator it;
            it = list.begin();
            for ( ; it != list.end(); it++ ) {
                String name = object_instance->name();
                char obj_inst_id[10];
                sprintf(obj_inst_id,"%d",object_instance->instance_id());

                // Append object instance id to the object name.
                name+= String("/");
                name+= String(obj_inst_id);

                char inst_id[10];
                sprintf(inst_id,"%d",(*it)->instance_id());

                // Append object instance id to the object name.
                name+= String("/");
                name+= (*it)->name();
                // if there are multiple instances supported
                // then add instance Id into creating resource path
                // else normal /object_id/object_instance/resource_id format.

                if((*it)->supports_multiple_instances()) {
                    name+= String("/") ;
                    name+= String(inst_id);
                 }
                if(name == resource_instance){
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

uint8_t M2MNsdlInterface::handle_get_request(sn_coap_hdr_s *received_coap_header,
                                             M2MBase *base,
                                             sn_nsdl_addr_s *address)
{
    uint8_t result = 1;
    if(base) {
        switch(base->base_type()) {
            case M2MBase::Object:
            {
                M2MObject* obj = (M2MObject*)base;
                result = handle_object_get_request(received_coap_header,obj,address);
            }
            break;
            case M2MBase::ObjectInstance:
            {
                M2MObjectInstance* instance = (M2MObjectInstance*)base;
                result = handle_object_instance_get_request(received_coap_header,instance,address);
            }
            break;
            case M2MBase::Resource:
            {
                M2MResource* resource = (M2MResource*)base;
                result = handle_resource_get_request(received_coap_header,resource,address);
            }
            break;
        }
    }
    return result;
}

uint8_t M2MNsdlInterface::handle_put_request(sn_coap_hdr_s *received_coap_header,
                                             M2MBase *base,
                                             sn_nsdl_addr_s *address)
{
    uint8_t result = 1;
    if(base) {
        switch(base->base_type()) {
            case M2MBase::Object:
            {
                M2MObject* obj = (M2MObject*)base;
                result = handle_object_put_request(received_coap_header,obj,address);
            }
            break;
            case M2MBase::ObjectInstance:
            {
                M2MObjectInstance* instance = (M2MObjectInstance*)base;
                result = handle_object_instance_put_request(received_coap_header,instance,address);
            }
            break;
            case M2MBase::Resource:
            {
                M2MResource* resource = (M2MResource*)base;
                result = handle_resource_put_request(received_coap_header,resource,address);
            }
            break;
        }
    }
    return result;
}

uint8_t M2MNsdlInterface::handle_post_request(sn_coap_hdr_s *received_coap_header,
                                              M2MBase *base,
                                              sn_nsdl_addr_s *address)
{
    uint8_t result = 1;
    if(base) {
        switch(base->base_type()) {
            case M2MBase::Object:
            {
                M2MObject* obj = (M2MObject*)base;
                result = handle_object_post_request(received_coap_header,obj,address);
            }
            break;
            case M2MBase::ObjectInstance:
            {
                M2MObjectInstance* instance = (M2MObjectInstance*)base;
                result = handle_object_instance_post_request(received_coap_header,instance,address);
            }
            break;
            case M2MBase::Resource:
            {
                M2MResource* resource = (M2MResource*)base;
                result = handle_resource_post_request(received_coap_header,resource,address);
            }
            break;
        }
    }
    return result;
}

uint8_t M2MNsdlInterface::handle_resource_get_request(sn_coap_hdr_s *received_coap_header,
                                                      M2MResource *resource,
                                                      sn_nsdl_addr_s *address)
{
    uint8_t result = 1;
    sn_coap_hdr_s * coap_response = NULL;
    uint8_t *value = 0;
    uint32_t length = 0;
    if(received_coap_header) {
        // process the GET if we have registered a callback for it
        if (resource && ((resource->operation() & SN_GRS_GET_ALLOWED) != 0)) {
            coap_response = sn_nsdl_build_response(_nsdl_handle,
                                                   received_coap_header,
                                                   COAP_MSG_CODE_RESPONSE_CONTENT);
            if(coap_response) {
                char content_type[10];
                int content_type_size = sprintf(content_type,"%d",resource->coap_content_type());

                coap_response->content_type_ptr = (uint8_t*)memory_alloc(content_type_size);
                if(coap_response->content_type_ptr) {
                    memcpy(coap_response->content_type_ptr,content_type,content_type_size);
                    coap_response->content_type_len = (uint8_t)content_type_size;
                }

                // call the resource get() to get value
                resource->get_value(value,length);

                // fill in the CoAP response payload
                coap_response->payload_len = length;
                coap_response->payload_ptr = value;

                coap_response->options_list_ptr = (sn_coap_options_list_s*)memory_alloc(sizeof(sn_coap_options_list_s));
                memset(coap_response->options_list_ptr, 0, sizeof(sn_coap_options_list_s));

                coap_response->options_list_ptr->observe_ptr = 0;

                if(received_coap_header->token_ptr) {
                    resource->set_observation_token(received_coap_header->token_ptr,
                                                    received_coap_header->token_len);
                }

                if(received_coap_header->options_list_ptr) {
                    if(received_coap_header->options_list_ptr->observe) {
                        uint32_t number = 0;
                        uint8_t observe_option = 0;
                        if(received_coap_header->options_list_ptr->observe_ptr) {
                            observe_option = *received_coap_header->options_list_ptr->observe_ptr;
                        }
                        if(START_OBSERVATION == observe_option) {

                            // If the observe length is 0 means register for observation.
                            if(received_coap_header->options_list_ptr->observe_len == 0) {
                                resource->set_under_observation(true,this);
                            }
                            else {
                            for(int i=0;i < received_coap_header->options_list_ptr->observe_len; i++) {
                                number = (*(received_coap_header->options_list_ptr->observe_ptr + i) & 0xff) <<
                                         8*(received_coap_header->options_list_ptr->observe_len- 1 - i);
                                }
                            }
                            // If the observe value is 0 means register for observation.
                            if(number == 0) {
                                resource->set_under_observation(true,this);
                                uint8_t observation_number[2];
                                uint8_t observation_number_length = 1;

                                uint16_t number = resource->observation_number();

                                observation_number[0] = ((number>>8) & 0xFF);
                                observation_number[1] = (number & 0xFF);

                                if(number > 0xFF) {
                                    observation_number_length = 2;
                                }
                                coap_response->options_list_ptr->observe_ptr = observation_number;
                                coap_response->options_list_ptr->observe_len = observation_number_length;
                            } else if(number == 1) {
                                // If the observe value is 1 means de-register from observation.
                                resource->set_under_observation(false,NULL);
                            }
                        } else if (STOP_OBSERVATION == observe_option) {
                            resource->set_under_observation(false,NULL);
                        }
                    }

                }
            }
        }else {
            // Operation is not allowed.
            coap_response = sn_nsdl_build_response(_nsdl_handle,
                                                   received_coap_header,
                                                   COAP_MSG_CODE_RESPONSE_BAD_REQUEST);
            if(coap_response) {
                coap_response->options_list_ptr = 0;
                coap_response->content_type_ptr = 0;
            }
        }
        if(coap_response) {
            // build response and send
            (sn_nsdl_send_coap_message(_nsdl_handle,
                                       address,
                                       coap_response) == 0) ? result = 0 : result = 1;
            if(coap_response->options_list_ptr) {
                coap_response->options_list_ptr->observe_ptr = 0;
                coap_response->options_list_ptr->observe_len = 0;
            }
            sn_nsdl_release_allocated_coap_msg_mem(_nsdl_handle,coap_response);
        }
    }
    if(value) {
        free(value);
    }
    return result;
}

uint8_t M2MNsdlInterface::handle_resource_put_request(sn_coap_hdr_s *received_coap_header,
                                                      M2MResource *resource,
                                                      sn_nsdl_addr_s *address)
{
    uint8_t result = 1;
    sn_coap_hdr_s *coap_response = NULL;
    // process the PUT if we have registered a callback for it
    if(received_coap_header) {
        if (resource && ((resource->operation() & SN_GRS_PUT_ALLOWED) != 0)) {
            sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_CHANGED; // 2.04
            resource->set_value(received_coap_header->payload_ptr, received_coap_header->payload_len);
            if(received_coap_header->payload_ptr) {
                _observer.value_updated(resource);
            }
            if(received_coap_header->options_list_ptr &&
               received_coap_header->options_list_ptr->uri_query_ptr) {
                char *query = (char*)memory_alloc(received_coap_header->options_list_ptr->uri_query_len+1);
                if (query){
                    memcpy(query,
                        received_coap_header->options_list_ptr->uri_query_ptr,
                        received_coap_header->options_list_ptr->uri_query_len);
                    memset(query + received_coap_header->options_list_ptr->uri_query_len,'\0',1);//String terminator
                    // if anything was updated, re-initialize the stored notification attributes
                    if (!resource->handle_observation_attribute(query)){
                        msg_code = COAP_MSG_CODE_RESPONSE_BAD_REQUEST; // 4.00
                    }
                    memory_free(query);
                }
            }
            coap_response = sn_nsdl_build_response(_nsdl_handle,
                                                   received_coap_header,
                                                   msg_code);
        } else {
            // Operation is not allowed.
            coap_response = sn_nsdl_build_response(_nsdl_handle,
                                                   received_coap_header,
                                                   COAP_MSG_CODE_RESPONSE_BAD_REQUEST);
            if(coap_response) {
                coap_response->options_list_ptr = 0;
                coap_response->content_type_ptr = 0;
            }
        }
        if(coap_response) {
            // build response and send
            (sn_nsdl_send_coap_message(_nsdl_handle,
                                       address,
                                       coap_response) == 0) ? result = 0 : result = 1;
            if(coap_response->options_list_ptr) {
                coap_response->options_list_ptr->observe_ptr = 0;
                coap_response->options_list_ptr->observe_len = 0;
            }
            sn_nsdl_release_allocated_coap_msg_mem(_nsdl_handle,coap_response);
        }
    }
    return result;
}

uint8_t M2MNsdlInterface::handle_resource_post_request(sn_coap_hdr_s *received_coap_header,
                                                       M2MResource *resource,
                                                       sn_nsdl_addr_s *address)
{
    uint8_t result = 1;
    sn_coap_hdr_s * coap_response = NULL;
    sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_CHANGED; // 2.04
    // process the POST if we have registered a callback for it
    if(resource && received_coap_header) {
        if ((resource->operation() & SN_GRS_POST_ALLOWED) != 0) {
            void *arguments = NULL;
            if(received_coap_header->options_list_ptr) {
                if(received_coap_header->options_list_ptr->uri_query_ptr) {
                    arguments = (void*)memory_alloc(received_coap_header->options_list_ptr->uri_query_len+1);
                    memset(arguments, 0, sizeof(received_coap_header->options_list_ptr->uri_query_len+1));
                    if (arguments){
                        memcpy(arguments,
                            received_coap_header->options_list_ptr->uri_query_ptr,
                            received_coap_header->options_list_ptr->uri_query_len);
                    }
                }
            }
            resource->execute(arguments);
            memory_free(arguments);
        } else { // if ((object->operation() & SN_GRS_POST_ALLOWED) != 0)
            msg_code = COAP_MSG_CODE_RESPONSE_METHOD_NOT_ALLOWED; // 4.05
        }
    } else { //if(object && received_coap_header)
        msg_code = COAP_MSG_CODE_RESPONSE_NOT_FOUND; // 4.01
    }
    coap_response = sn_nsdl_build_response(_nsdl_handle,
                                           received_coap_header,
                                           msg_code);
    if(coap_response) {
        (sn_nsdl_send_coap_message(_nsdl_handle, address, coap_response) == 0) ? result = 0 : result = 1;
        sn_nsdl_release_allocated_coap_msg_mem(_nsdl_handle, coap_response);
    }
    return result;
}

uint8_t M2MNsdlInterface::handle_object_instance_get_request(sn_coap_hdr_s *received_coap_header,
                                                             M2MObjectInstance */*instance*/,
                                                             sn_nsdl_addr_s *address)
{
    uint8_t result = 1;
    sn_coap_hdr_s * coap_response = NULL;
    //TODO: GET for ObjectInstance is not yet implemented.
    // Need to first fix C library and then implement on C++ side.
    sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_NOT_IMPLEMENTED; // 5.01
    if(received_coap_header) {
        coap_response = sn_nsdl_build_response(_nsdl_handle,
                                               received_coap_header,
                                               msg_code);
        if(coap_response) {
            (sn_nsdl_send_coap_message(_nsdl_handle, address, coap_response) == 0) ? result = 0 : result = 1;
            sn_nsdl_release_allocated_coap_msg_mem(_nsdl_handle, coap_response);
        }
    }
    return result;
}

uint8_t M2MNsdlInterface::handle_object_instance_put_request(sn_coap_hdr_s *received_coap_header,
                                                             M2MObjectInstance */*instance*/,
                                                             sn_nsdl_addr_s *address)
{
    uint8_t result = 1;
    sn_coap_hdr_s * coap_response = NULL;
    //TODO: PUT for ObjectInstance is not yet implemented.
    // Need to first fix C library and then implement on C++ side.
    sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_NOT_IMPLEMENTED; // 5.01
    if(received_coap_header) {
        coap_response = sn_nsdl_build_response(_nsdl_handle,
                                               received_coap_header,
                                               msg_code);
        if(coap_response) {
            (sn_nsdl_send_coap_message(_nsdl_handle, address, coap_response) == 0) ? result = 0 : result = 1;
            sn_nsdl_release_allocated_coap_msg_mem(_nsdl_handle, coap_response);
        }
    }
    return result;
}

uint8_t M2MNsdlInterface::handle_object_instance_post_request(sn_coap_hdr_s *received_coap_header,
                                                              M2MObjectInstance */*instance*/,
                                                              sn_nsdl_addr_s *address)
{
    uint8_t result = 1;
    sn_coap_hdr_s * coap_response = NULL;
    //TODO: POST for ObjectInstance is not yet implemented.
    // Need to first fix C library and then implement on C++ side.
    sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_NOT_IMPLEMENTED; // 5.01
    if(received_coap_header) {
        coap_response = sn_nsdl_build_response(_nsdl_handle,
                                               received_coap_header,
                                               msg_code);
        if(coap_response) {
            (sn_nsdl_send_coap_message(_nsdl_handle, address, coap_response) == 0) ? result = 0 : result = 1;
            sn_nsdl_release_allocated_coap_msg_mem(_nsdl_handle, coap_response);
        }
    }
    return result;
}

uint8_t M2MNsdlInterface::handle_object_get_request(sn_coap_hdr_s *received_coap_header,
                                                    M2MObject */*object*/,
                                                    sn_nsdl_addr_s *address)
{
    uint8_t result = 1;
    sn_coap_hdr_s * coap_response = NULL;
    //TODO: GET for Object is not yet implemented.
    // Need to first fix C library and then implement on C++ side.
    sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_NOT_IMPLEMENTED; // 5.01
    if(received_coap_header) {
        coap_response = sn_nsdl_build_response(_nsdl_handle,
                                               received_coap_header,
                                               msg_code);
        if(coap_response) {
            (sn_nsdl_send_coap_message(_nsdl_handle, address, coap_response) == 0) ? result = 0 : result = 1;
            sn_nsdl_release_allocated_coap_msg_mem(_nsdl_handle, coap_response);
        }
    }
    return result;
}

uint8_t M2MNsdlInterface::handle_object_put_request(sn_coap_hdr_s *received_coap_header,
                                                    M2MObject */*object*/,
                                                    sn_nsdl_addr_s *address)
{
    uint8_t result = 1;
    sn_coap_hdr_s * coap_response = NULL;
    //TODO: PUT for Object is not yet implemented.
    // Need to first fix C library and then implement on C++ side.
    sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_NOT_IMPLEMENTED; // 5.01
    if(received_coap_header) {
        coap_response = sn_nsdl_build_response(_nsdl_handle,
                                               received_coap_header,
                                               msg_code);
        if(coap_response) {
            (sn_nsdl_send_coap_message(_nsdl_handle, address, coap_response) == 0) ? result = 0 : result = 1;
            sn_nsdl_release_allocated_coap_msg_mem(_nsdl_handle, coap_response);
        }
    }
    return result;
}

uint8_t M2MNsdlInterface::handle_object_post_request(sn_coap_hdr_s *received_coap_header,
                                                     M2MObject */*object*/,
                                                     sn_nsdl_addr_s *address)
{
    uint8_t result = 1;
    sn_coap_hdr_s * coap_response = NULL;
    //TODO: POST for Object is not yet implemented.
    // Need to first fix C library and then implement on C++ side.
    sn_coap_msg_code_e msg_code = COAP_MSG_CODE_RESPONSE_NOT_IMPLEMENTED; // 5.01
    if(received_coap_header) {
        coap_response = sn_nsdl_build_response(_nsdl_handle,
                                               received_coap_header,
                                               msg_code);
        if(coap_response) {
            (sn_nsdl_send_coap_message(_nsdl_handle, address, coap_response) == 0) ? result = 0 : result = 1;
            sn_nsdl_release_allocated_coap_msg_mem(_nsdl_handle, coap_response);
        }
    }
    return result;
}

void M2MNsdlInterface::clear_resource(sn_nsdl_resource_info_s *&resource)
{
    //Clear up the filled resource to fill up new resource.
    if(resource && resource->resource_parameters_ptr) {
        sn_nsdl_resource_parameters_s *temp_resource_parameter = resource->resource_parameters_ptr;
        memset(resource->resource_parameters_ptr, 0, sizeof(sn_nsdl_resource_parameters_s));
        memset(resource,0, sizeof(sn_nsdl_resource_info_s));
        resource->resource_parameters_ptr = temp_resource_parameter;
    }
}
