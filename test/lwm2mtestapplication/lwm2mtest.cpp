/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "lwm2mtest.h"
#include "ns_types.h"
#include "ns_cmdline.h"

M2MLWClient::M2MLWClient()
: _security(NULL),
  _interface(NULL),
  _device(NULL),
  _bootstrapped(false),
  _error(false),
  _registered(false),
  _unregistered(false),
  _registration_updated(false)
{
}

M2MLWClient::~M2MLWClient()
{
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


bool M2MLWClient::create_interface(int32_t param_count,
					  const char *endpoint,
                      const char *resource_type,
                      const int32_t lifetime,
                      const uint16_t listen_port,
                      const char *domain,
                      const uint8_t binding_mode,
                      const uint8_t network_interface)
{
    if(_interface) {
        delete _interface;
        _interface = NULL;
    }
    String ep;
    String rt;
    String dmn;
    if(endpoint) {
        ep += endpoint;
    }
    if(resource_type) {
        rt += resource_type;
    }
    if(domain) {
        dmn += domain;
    }

    // Binding mode cannot be higher than 0x07 since it is an enum, check M2MInterface::BindingMode
    if(binding_mode > 0x07) {
        return false;
    }

    switch (param_count) {
		case 0:
			_interface = M2MInterfaceFactory::create_interface(*this, ep, rt);
			break;
		case 1:
			_interface = M2MInterfaceFactory::create_interface(*this, ep, rt, lifetime);
			break;
		case 2:
			_interface = M2MInterfaceFactory::create_interface(*this, ep, rt, lifetime, listen_port);
			break;
		case 3:
			_interface = M2MInterfaceFactory::create_interface(*this, ep, rt, lifetime, listen_port, dmn);
			break;
		case 4:
			_interface = M2MInterfaceFactory::create_interface(*this, ep, rt, lifetime, listen_port, dmn, (M2MInterface::BindingMode)binding_mode);
			break;
		case 5:
			_interface = M2MInterfaceFactory::create_interface(*this, ep, rt, lifetime, listen_port, dmn, (M2MInterface::BindingMode)binding_mode, (M2MInterface::NetworkStack)network_interface);
			break;
	}
    return (_interface == NULL) ? false : true;
}

bool M2MLWClient::create_bootstrap_object(const char *coap_bootstrap_address)
{
    bool success = false;
    String address;
    if(coap_bootstrap_address) {
        address += coap_bootstrap_address;
    }
    if(_security) {
        delete _security;
    }
    _security = M2MInterfaceFactory::create_security(M2MSecurity::Bootstrap);
    if(_security) {
        if(_security->set_resource_value(M2MSecurity::M2MServerUri, address) &&
           _security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::NoSecurity)) {
            success = true;
        }
    }
    return success;
}

bool M2MLWClient::create_register_object(const char *coap_register_address)
{
    bool success = false;
    String address;
    if(coap_register_address) {
        address += coap_register_address;
    }
    if(_register_security) {
        delete _register_security;
    }
    _register_security = M2MInterfaceFactory::create_security(M2MSecurity::M2MServer);
    if(_register_security) {
        if(_register_security->set_resource_value(M2MSecurity::M2MServerUri, address) &&
           _register_security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::NoSecurity)) {
            success = true;
        }
    }
    return success;
}

bool M2MLWClient::test_bootstrap()
{
    bool success = false;
    if(_interface) {
        _interface->bootstrap(_security);
        success = true;
    }
    return success;
}

bool M2MLWClient::create_device_object(M2MDevice::DeviceResource resource,
                                       const char *value)
{
    bool success = false;
    String value_string;
    if(value) {
        value_string += value;
    }
    if(!_device) {
        _device = M2MInterfaceFactory::create_device();
    }
    if(_device) {

        if(_device->create_resource(resource,value_string)){
            success = true;
        } else {
            success = _device->set_resource_value(resource,value);
        }
    }
    return success;
}

bool M2MLWClient::create_device_object()
{
    bool success = false;
    if(!_device) {
        _device = M2MInterfaceFactory::create_device();
        success = true;
    }
    return success;
}

bool M2MLWClient::create_device_object(M2MDevice::DeviceResource resource,
                          uint32_t value)
{
    bool success = false;
    if(!_device) {
        _device = M2MInterfaceFactory::create_device();
    }
    if(_device) {
        if(_device->create_resource(resource,value)) {
            success = true;
        } else {
            success = _device->set_resource_value(resource,value);
        }
    }
    return success;
}

bool M2MLWClient::create_object(const char *name, bool new_instance)
{
    bool success = false;
    if(!_object) {
        _object = M2MInterfaceFactory::create_object(name);
        if(_object) {
            if(_object->create_object_instance()) {
                success = true;
            }
        }
    } else {
        if(new_instance) {
            if(_object->create_object_instance()) {
                success = true;
            }
        }
    }
    return success;
}

bool M2MLWClient::create_static_resource_string(const char *name,
                                                const char *value,
                                                bool multiple_instance,
                                                uint16_t object_instance)
{
    bool success = false;
    String name_string;
    if(name) {
        name_string += name;
    }
    String value_string;
    if(value) {
        value_string += value;
    }
    if(_object) {
        M2MObjectInstance *inst = _object->object_instance(object_instance);
        if(inst) {
            if(inst->create_static_resource(name,"resource",
                                            M2MResourceInstance::STRING,
                                            (const uint8_t*)value_string.c_str(),
                                            value_string.size()) != NULL) {
                success = true;
            }
        }
    }
    return success;
}

bool M2MLWClient::create_static_resource_int(const char *name,
                                             uint32_t value,
                                             bool multiple_instance,
                                             uint16_t object_instance)
{
    bool success = false;
    String name_string;
    String value_string;

    if(name) {
        name_string += name;
    }

    char value_buffer[20];
    sprintf(value_buffer,"%d",value);
    value_string += value_buffer;

    if(_object) {
        M2MObjectInstance *inst = _object->object_instance(object_instance);
        if(inst) {
            if(inst->create_static_resource(name,"resource",
                                            M2MResourceInstance::INTEGER,
                                            (const uint8_t*)value_string.c_str(),
                                            value_string.size()) != NULL) {
                success = true;
            }
        }
    }
    return success;
}

bool M2MLWClient::create_dynamic_resource(const char *name,
                                          bool observable,
                                          bool multiple_instance,
                                          uint16_t object_instance)
{
    bool success = false;
    String name_string;
    if(name) {
        name_string += name;
    }
    if(_object) {
        M2MObjectInstance *inst = _object->object_instance(object_instance);
        if(inst) {
            if(inst->create_dynamic_resource(name,"resource",
                                             M2MResourceInstance::OPAQUE,
                                             observable) != NULL) {
                success = true;
            }
        }
    }
    return success;
}

bool M2MLWClient::set_resource_value(const char *name,
                                          int32_t value,
                                          uint16_t object_instance)
{
    bool success = false;
    String name_string;
    String value_string;
    if(name) {
        name_string += name;
    }

    char value_buffer[20];
    sprintf(value_buffer,"%d",value);
    value_string += value_buffer;

    if(_object && name_string.length() > 0) {
        M2MObjectInstance *inst = _object->object_instance(object_instance);
        if(inst) {
            M2MResource *res = inst->resource(name_string);
            if (res) {
                if (res->set_value((const uint8_t*)value_string.c_str(), value_string.size())) {
                    success = true;
                }
            }
        }
    }
    return success;
}

bool M2MLWClient::test_register()
{
    bool success = false;
    M2MObjectList object_list;
    if(_device) {
        object_list.push_back(_device);
    }
    if(_object) {
        object_list.push_back(_object);
    }
    if(_interface) {
        _interface->register_object(_register_security,object_list);
        success = true;
    }
    return success;
}

bool M2MLWClient::test_update_register(const uint32_t lifetime)
{
    bool success = false;
    if(_interface && _register_security) {
        success = true;
        _interface->update_registration(_register_security,lifetime);
    }
    return success;
}

bool M2MLWClient::test_unregister()
{
    bool success = false;
    if(_interface) {
         success = true;
        _interface->unregister_object(_register_security);
    }
    return success;
}

void M2MLWClient::bootstrap_done(M2MSecurity *server_object)
{
    if(server_object) {
        _register_security = server_object;
        _bootstrapped = true;
        cmd_printf("\nBootstrapped\n");
        cmd_ready( CMDLINE_RETCODE_SUCCESS );
    }
}

void M2MLWClient::object_registered(M2MSecurity *security_object, const M2MServer &server_object)
{
    _registered = true;
    cmd_printf("\nRegistered\n");
    cmd_ready( CMDLINE_RETCODE_SUCCESS );
}

void M2MLWClient::object_unregistered(M2MSecurity *server_object)
{
    _unregistered = true;
    if(_device) {
        M2MDevice::delete_instance();
        _device = NULL;
    }
    if(_object) {
        delete _object;
        _object = NULL;
    }
    if(_security) {
        delete _security;
        _security = NULL;
    }
    if(_register_security) {
        delete _register_security;
        _register_security = NULL;
    }
    cmd_printf("\nUnregistered\n");
    cmd_ready( CMDLINE_RETCODE_SUCCESS );
}

void M2MLWClient::registration_updated(M2MSecurity *security_object,
                                       const M2MServer &server_object)
{
    _registration_updated = true;
    cmd_printf("\nregistration updated\n");
    cmd_ready( CMDLINE_RETCODE_SUCCESS );
}

void M2MLWClient::error(M2MInterface::Error error)
{
    _error = true;
    cmd_printf("\nError occured Error Code : %d\n", (int8_t)error);
    cmd_ready( CMDLINE_RETCODE_SUCCESS );
}

void M2MLWClient::value_updated(M2MBase *base, M2MBase::BaseType type)
{
    cmd_printf("\nValue updated of Object name %s and Type \n",
               base->name().c_str(), type);
}
