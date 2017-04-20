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
#include <stdlib.h>
#include "mbed-client/m2mresource.h"
#include "mbed-client/m2mconstants.h"
#include "mbed-client/m2mobservationhandler.h"
#include "mbed-client/m2mobject.h"
#include "mbed-client/m2mobjectinstance.h"
#include "include/m2mcallbackstorage.h"
#include "include/m2mreporthandler.h"
#include "mbed-client/m2mblockmessage.h"
#include "mbed-trace/mbed_trace.h"

#define TRACE_GROUP "mClt"

M2MResourceInstance::M2MResourceInstance(M2MResource &parent,
                                         const String &res_name,
                                         M2MBase::Mode resource_mode,
                                         const String &resource_type,
                                         M2MBase::DataType type,
                                         char* path,
                                         bool external_blockwise_store,
                                         bool multiple_instance)
: M2MResourceBase(res_name,
          resource_mode,
          resource_type,
          type,
          path,
          external_blockwise_store,
          multiple_instance
          ),
 _parent_resource(parent)
{
    set_base_type(M2MBase::ResourceInstance);
}

M2MResourceInstance::M2MResourceInstance(M2MResource &parent,
                                         const String &res_name,
                                         M2MBase::Mode resource_mode,
                                         const String &resource_type,
                                         M2MBase::DataType type,
                                         const uint8_t *value,
                                         const uint8_t value_length,
                                         char* path,
                                         bool external_blockwise_store,
                                         bool multiple_instance)
: M2MResourceBase(res_name,
          resource_mode,
          resource_type,
          type,
          value,
          value_length,
          path,
          external_blockwise_store,
          multiple_instance),
 _parent_resource(parent)
{
    set_base_type(M2MBase::ResourceInstance);
}

M2MResourceInstance::M2MResourceInstance(M2MResource &parent,
                                         const lwm2m_parameters_s* s,
                                         M2MBase::DataType type)
: M2MResourceBase(s, type),
  _parent_resource(parent)
{

    assert(base_type() == M2MBase::ResourceInstance);
}

M2MResourceInstance::~M2MResourceInstance()
{
    free_resources();
}

M2MObservationHandler* M2MResourceInstance::observation_handler() const
{
    const M2MResource& parent_resource = get_parent_resource();

    // XXX: need to check the flag too
    return parent_resource.observation_handler();
}

void M2MResourceInstance::set_observation_handler(M2MObservationHandler *handler)
{
    M2MResource& parent_resource = get_parent_resource();

    // XXX: need to set the flag too
    parent_resource.set_observation_handler(handler);
}

bool M2MResourceInstance::handle_observation_attribute(const char *query)
{
    tr_debug("M2MResourceInstance::handle_observation_attribute - is_under_observation(%d)", is_under_observation());
    bool success = false;

    M2MReportHandler *handler = M2MBase::report_handler();
    if (!handler) {
        handler = M2MBase::create_report_handler();
    }

    if (handler) {
        success = handler->parse_notification_attribute(query,
                M2MBase::base_type(), resource_instance_type());
        if(success) {
            if (is_under_observation()) {
                handler->set_under_observation(true);
            }
        } else {
            handler->set_default_values();
        }
    }
    return success;
}

uint16_t M2MResourceInstance::object_instance_id() const
{
    const M2MObjectInstance& parent_object_instance = get_parent_resource().get_parent_object_instance();
    return parent_object_instance.instance_id();
}

M2MResource& M2MResourceInstance::get_parent_resource() const
{
    return _parent_resource;
}

const char* M2MResourceInstance::object_name() const
{
    const M2MObjectInstance& parent_object_instance = _parent_resource.get_parent_object_instance();
    const M2MObject& parent_object = parent_object_instance.get_parent_object();

    return parent_object.name();
}

