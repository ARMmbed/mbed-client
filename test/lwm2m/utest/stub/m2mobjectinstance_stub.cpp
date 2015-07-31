/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "m2mobjectinstance_stub.h"

u_int8_t m2mobjectinstance_stub::int_value;
bool m2mobjectinstance_stub::bool_value;
M2MResource* m2mobjectinstance_stub::resource;
M2MResource* m2mobjectinstance_stub::create_resource;
M2MResourceList m2mobjectinstance_stub::resource_list;
M2MResourceInstance* m2mobjectinstance_stub::create_resource_instance;
sn_coap_hdr_s* m2mobjectinstance_stub::header;


void m2mobjectinstance_stub::clear()
{
    int_value = 0;
    bool_value = false;
    resource = NULL;
    create_resource = NULL;
    create_resource_instance = NULL;
    header = NULL;
    resource_list.clear();
}

M2MObjectInstance& M2MObjectInstance::operator=(const M2MObjectInstance& )
{
    return *this;
}

M2MObjectInstance::M2MObjectInstance(const M2MObjectInstance& other)
: M2MBase(other)
{
    *this = other;
}

M2MObjectInstance::M2MObjectInstance(const String &object_name)
: M2MBase(object_name,M2MBase::Dynamic)
{
}

M2MObjectInstance::~M2MObjectInstance()
{
}

M2MResource* M2MObjectInstance::create_static_resource(const String &,
                                               const String &,
                                               M2MResourceInstance::ResourceType,
                                               const uint8_t *,
                                               const uint8_t,
                                               bool)
{
    return m2mobjectinstance_stub::create_resource;
}

M2MResource* M2MObjectInstance::create_dynamic_resource(const String &,
                                                const String &,
                                                M2MResourceInstance::ResourceType,
                                                bool,
                                                bool)
{
    return m2mobjectinstance_stub::create_resource;
}

M2MResourceInstance* M2MObjectInstance::create_static_resource_instance(const String &,
                                               const String &,
                                               M2MResourceInstance::ResourceType,
                                               const uint8_t *,
                                               const uint8_t,
                                               uint16_t)
{
    return m2mobjectinstance_stub::create_resource_instance;
}

M2MResourceInstance* M2MObjectInstance::create_dynamic_resource_instance(const String &,
                                                const String &,
                                                M2MResourceInstance::ResourceType,
                                                bool,
                                                uint16_t)
{
    return m2mobjectinstance_stub::create_resource_instance;
}

bool M2MObjectInstance::remove_resource(const String &)
{
    return m2mobjectinstance_stub::bool_value;
}

bool M2MObjectInstance::remove_resource_instance(const String &, uint16_t)
{
    return m2mobjectinstance_stub::bool_value;
}

M2MResource* M2MObjectInstance::resource(const String &) const
{
    return m2mobjectinstance_stub::resource;
}

const M2MResourceList& M2MObjectInstance::resources() const
{
    return m2mobjectinstance_stub::resource_list;
}

uint16_t M2MObjectInstance::resource_count() const
{
    return m2mobjectinstance_stub::int_value;
}

uint16_t M2MObjectInstance::resource_count(const String& ) const
{
    return m2mobjectinstance_stub::int_value;
}

M2MBase::BaseType M2MObjectInstance::base_type() const
{
    return M2MBase::ObjectInstance;
}

bool M2MObjectInstance::handle_observation_attribute(char *&){
    return m2mobjectinstance_stub::bool_value;
}

sn_coap_hdr_s* M2MObjectInstance::handle_get_request(nsdl_s *,
                                  sn_coap_hdr_s *,
                                  M2MObservationHandler *)
{
    return m2mobjectinstance_stub::header;
}

sn_coap_hdr_s* M2MObjectInstance::handle_put_request(nsdl_s *,
                                  sn_coap_hdr_s *,
                                  M2MObservationHandler *)
{
    return m2mobjectinstance_stub::header;
}

sn_coap_hdr_s* M2MObjectInstance::handle_post_request(nsdl_s *,
                                   sn_coap_hdr_s *,
                                   M2MObservationHandler *)
{
    return m2mobjectinstance_stub::header;
}
