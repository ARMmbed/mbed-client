/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include <cstdio>
#include "lwm2m-client/m2mserver.h"
#include "lwm2m-client/m2mconstants.h"
#include "lwm2m-client/m2mobject.h"
#include "lwm2m-client/m2mobjectinstance.h"
#include "lwm2m-client/m2mresource.h"


M2MServer::M2MServer()
: M2MObject(M2M_SERVER_ID)
{
    M2MObject::create_object_instance();

    _server_instance = object_instance();

    if(_server_instance) {

        M2MResource* res = _server_instance->create_dynamic_resource(SERVER_SHORT_SERVER_ID,OMA_RESOURCE_TYPE,true);
        if(res) {
            res->set_operation(M2MBase::GET_PUT_ALLOWED);
        }
        res = _server_instance->create_dynamic_resource(SERVER_LIFETIME,OMA_RESOURCE_TYPE,true);
        if(res) {
            res->set_operation(M2MBase::GET_PUT_POST_ALLOWED);
        }
        res = _server_instance->create_dynamic_resource(SERVER_NOTIFICATION_STORAGE,OMA_RESOURCE_TYPE,true);
        if(res) {
            res->set_operation(M2MBase::GET_PUT_POST_ALLOWED);
        }
        res = _server_instance->create_dynamic_resource(SERVER_BINDING,OMA_RESOURCE_TYPE,true);
        if(res) {
            res->set_operation(M2MBase::GET_PUT_POST_ALLOWED);
        }
        res = _server_instance->create_dynamic_resource(SERVER_REGISTRATION_UPDATE,OMA_RESOURCE_TYPE,false);
        if(res) {
          res->set_operation(M2MBase::POST_ALLOWED);
        }
    }
}

M2MServer::~M2MServer()
{

}

M2MResource* M2MServer::create_resource(ServerResource resource, uint32_t value)
{
    M2MResource* res = NULL;
    String server_id = "";
    if(!is_resource_present(resource)) {
        switch(resource) {
        case DefaultMinPeriod:
            server_id = SERVER_DEFAULT_MIN_PERIOD;
            break;
        case DefaultMaxPeriod:
            server_id = SERVER_DEFAULT_MAX_PERIOD;
            break;
        case DisableTimeout:
            server_id = SERVER_DISABLE_TIMEOUT;
            break;
        default:
            break;
        }
    }
    if(!server_id.empty()) {
        if(_server_instance) {
            res = _server_instance->create_dynamic_resource(server_id,OMA_RESOURCE_TYPE,true);
            if(res) {
                res->set_operation(M2MBase::GET_PUT_POST_ALLOWED);
                // If resource is created then set the value.
                char buffer[20];
                int size = sprintf(buffer,"%ld",(long int)value);
                res->set_value((const uint8_t*)buffer,(const uint32_t)size);
            }
        }
    }
    return res;
}

M2MResource* M2MServer::create_resource(ServerResource resource)
{
    M2MResource* res = NULL;
    if(!is_resource_present(resource)) {
        if(M2MServer::Disable == resource) {
                if(_server_instance) {
                    res = _server_instance->create_dynamic_resource(SERVER_DISABLE,OMA_RESOURCE_TYPE,false);
                if(res) {
                    res->set_operation(M2MBase::POST_ALLOWED);
                }
            }
        }
    }
    return res;
}

bool M2MServer::delete_resource(ServerResource resource)
{
    bool success = false;
    String server_id = "";
    switch(resource) {
        case DefaultMinPeriod:
           server_id = SERVER_DEFAULT_MIN_PERIOD;
           break;
        case DefaultMaxPeriod:
            server_id = SERVER_DEFAULT_MAX_PERIOD;
            break;
        case Disable:
            server_id = SERVER_DISABLE;
            break;
        case DisableTimeout:
            server_id = SERVER_DISABLE_TIMEOUT;
            break;
        default:
            break;
    }
    if(!server_id.empty()) {
        if(_server_instance) {
            success = _server_instance->remove_resource(server_id);
        }
    }
    return success;
}

bool M2MServer::set_resource_value(ServerResource resource,
                                   const String &value)
{
    bool success = false;
    M2MResource* res = get_resource(resource);
    if(res && (M2MServer::Binding == resource)) {
        success = res->set_value((const uint8_t*)value.c_str(),(uint32_t)value.length());
    }
    return success;
}

bool M2MServer::set_resource_value(ServerResource resource,
                                   uint32_t value)
{
    bool success = false;
    M2MResource* res = get_resource(resource);
    if(res) {
        if(M2MServer::ShortServerID == resource     ||
           M2MServer::Lifetime == resource          ||
           M2MServer::DefaultMinPeriod == resource  ||
           M2MServer::DefaultMaxPeriod == resource  ||
           M2MServer::DisableTimeout == resource    ||
           M2MServer::NotificationStorage == resource) {
            // If it is any of the above resource
            // set the value of the resource.
            char buffer[20];
            int size = sprintf(buffer,"%ld",(long int)value);
            success = res->set_value((const uint8_t*)buffer,
                                     (const uint32_t)size,
                                     true);
        }
    }
    return success;
}

String M2MServer::resource_value_string(ServerResource resource) const
{
    String value = "";
    M2MResource* res = get_resource(resource);
    if(res && (M2MServer::Binding == resource)) {
        uint8_t* buffer = NULL;
        uint32_t length = 0;
        res->get_value(buffer,length);

        char *char_buffer = (char*)malloc(length+1);
        if(char_buffer) {
            memset(char_buffer,0,length+1);
            memcpy(char_buffer,(char*)buffer,length);

            String s_name(char_buffer);
            value = s_name;
            if(buffer) {
                free(buffer);
            }
            if(char_buffer) {
                free(char_buffer);
            }
        }
    }
    return value;
}


uint32_t M2MServer::resource_value_int(ServerResource resource) const
{
    uint32_t value = 0;
    M2MResource* res = get_resource(resource);
    if(res) {
        if(M2MServer::ShortServerID == resource     ||
           M2MServer::Lifetime == resource          ||
           M2MServer::DefaultMinPeriod == resource  ||
           M2MServer::DefaultMaxPeriod == resource  ||
           M2MServer::DisableTimeout == resource    ||
           M2MServer::NotificationStorage == resource) {
            // Get the value and convert it into integer
            uint8_t* buffer = NULL;
            uint32_t length = 0;
            res->get_value(buffer,length);
            if(buffer) {
                value = atoi((const char*)buffer);
                free(buffer);
            }
        }
    }
    return value;
}

bool M2MServer::is_resource_present(ServerResource resource) const
{
    bool success = false;
    M2MResource *res = get_resource(resource);
    if(res) {
        success = true;
    }
    return success;
}

uint16_t M2MServer::total_resource_count() const
{
    uint16_t total_count = 0;    
    if(_server_instance) {
    total_count = _server_instance->resources().size();
    }
    return total_count;
}

M2MResource* M2MServer::get_resource(ServerResource res) const
{
    M2MResource* res_object = NULL;
    String res_name = "";
    switch(res) {
    case ShortServerID:
        res_name = SERVER_SHORT_SERVER_ID;
        break;
    case Lifetime:
        res_name = SERVER_LIFETIME;
        break;
    case DefaultMinPeriod:
        res_name = SERVER_DEFAULT_MIN_PERIOD;
        break;
    case DefaultMaxPeriod:
        res_name = SERVER_DEFAULT_MAX_PERIOD;
        break;
    case Disable:
        res_name = SERVER_DISABLE;
        break;
    case DisableTimeout:
        res_name = SERVER_DISABLE_TIMEOUT;
        break;
    case NotificationStorage:
        res_name = SERVER_NOTIFICATION_STORAGE;
        break;
    case Binding:
        res_name = SERVER_BINDING;
        break;
    case RegistrationUpdate:
        res_name = SERVER_REGISTRATION_UPDATE;
        break;
    }
    if(!res_name.empty()) {        
        if(_server_instance) {
        res_object = _server_instance->resource(res_name);
        }
    }
    return res_object;
}
