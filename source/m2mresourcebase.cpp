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
#if 1
#include <stdlib.h>
#include "mbed-client/m2mresourcebase.h"
#include "mbed-client/m2mconstants.h"
#include "mbed-client/m2mobservationhandler.h"
#include "mbed-client/m2mobject.h"
#include "mbed-client/m2mobjectinstance.h"
#include "include/m2mcallbackstorage.h"
#include "include/m2mreporthandler.h"
#include "include/nsdllinker.h"
#include "mbed-client/m2mblockmessage.h"
#include "mbed-trace/mbed_trace.h"

#define TRACE_GROUP "mClt"

M2MResourceBase::M2MResourceBase(
                                         const String &res_name,
                                         const String &resource_type,
                                         M2MBase::DataType type,
                                         char* path,
                                         bool external_blockwise_store,
                                         bool multiple_instance)
: M2MBase(res_name,
          M2MBase::Dynamic,
#ifndef DISABLE_RESOURCE_TYPE
          resource_type,
#endif
          path,
          external_blockwise_store,
          multiple_instance,
          type)
#ifndef DISABLE_BLOCK_MESSAGE
 ,_block_message_data(NULL)
#endif
{
}

M2MResourceBase::M2MResourceBase(
                                         const String &res_name,
                                         const String &resource_type,
                                         M2MBase::DataType type,
                                         const uint8_t *value,
                                         const uint8_t value_length,
                                         char* path,
                                         bool external_blockwise_store,
                                         bool multiple_instance)
: M2MBase(res_name,
          M2MBase::Static,
#ifndef DISABLE_RESOURCE_TYPE
          resource_type,
#endif
          path,
          external_blockwise_store,
          multiple_instance,
          type)
#ifndef DISABLE_BLOCK_MESSAGE
 ,_block_message_data(NULL)
#endif
{
    M2MBase::set_base_type(M2MBase::Resource);
    if (mode() == M2MBase::Dynamic) {
        if( value != NULL && value_length > 0 ) {
            sn_nsdl_dynamic_resource_parameters_s* res = get_nsdl_resource();
            res->resource = alloc_string_copy(value, value_length);
            res->resourcelen = value_length;
        }
    }
    // Copy resource value to struct since static resources are handled in mbed-client-c
    else if (mode() == M2MBase::Static) {
       sn_nsdl_dynamic_resource_parameters_s* res = get_nsdl_resource();
        res->resource = alloc_string_copy(value, value_length);
        res->resourcelen = value_length;
    }
    else {
        // Directory, not supported
    }
}

M2MResourceBase::M2MResourceBase(
                                         const lwm2m_parameters_s* s,
                                         M2MBase::DataType /*type*/)
: M2MBase(s)
#ifndef DISABLE_BLOCK_MESSAGE
  ,_block_message_data(NULL)
#endif
{
    //TBD: put to flash, or parse from the uri_path!!!!
    //same for the _object_instance_id.
    //M2MBase::set_base_type(M2MBase::ResourceInstance);
}

M2MResourceBase::~M2MResourceBase()
{
    execute_callback* callback = (execute_callback*)M2MCallbackStorage::remove_callback(*this,
                                    M2MCallbackAssociation::M2MResourceInstanceExecuteCallback);
    delete callback;

    M2MCallbackStorage::remove_callback(*this, M2MCallbackAssociation::M2MResourceInstanceExecuteCallback2);
#ifndef DISABLE_BLOCK_MESSAGE
    incoming_block_message_callback *in_callback = (incoming_block_message_callback*)M2MCallbackStorage::remove_callback(*this,
                                                        M2MCallbackAssociation::M2MResourceInstanceIncomingBlockMessageCallback);
    delete in_callback;

    outgoing_block_message_callback *out_callback = (outgoing_block_message_callback*)M2MCallbackStorage::remove_callback(*this,
                                                        M2MCallbackAssociation::M2MResourceInstanceOutgoingBlockMessageCallback);
    delete out_callback;
#endif

    notification_sent_callback *notif_callback = (notification_sent_callback*)M2MCallbackStorage::remove_callback(*this,
                                                        M2MCallbackAssociation::M2MResourceInstanceNotificationSentCallback);
    delete notif_callback;

    M2MCallbackStorage::remove_callback(*this, M2MCallbackAssociation::M2MResourceInstanceNotificationSentCallback2);

#ifndef DISABLE_BLOCK_MESSAGE
    delete _block_message_data;
#endif
}

M2MResourceBase::ResourceType M2MResourceBase::resource_instance_type() const
{
    M2MBase::lwm2m_parameters_s* param = M2MBase::get_lwm2m_parameters();
    M2MBase::DataType type = param->data_type;
    return convert_data_type(type);
}


bool M2MResourceBase::set_execute_function(execute_callback callback)
{
    execute_callback* old_callback = (execute_callback*)M2MCallbackStorage::remove_callback(*this, M2MCallbackAssociation::M2MResourceInstanceExecuteCallback);
    delete old_callback;
    // XXX: create a copy of the copy of callback object. Perhaps it would better to
    // give a reference as parameter and just store that, as it would save some memory.
    execute_callback* new_callback = new execute_callback(callback);

    return M2MCallbackStorage::add_callback(*this, new_callback, M2MCallbackAssociation::M2MResourceInstanceExecuteCallback);
}

bool M2MResourceBase::set_execute_function(execute_callback_2 callback)
{
    M2MCallbackStorage::remove_callback(*this, M2MCallbackAssociation::M2MResourceInstanceExecuteCallback2);

    return M2MCallbackStorage::add_callback(*this, (void*)callback, M2MCallbackAssociation::M2MResourceInstanceExecuteCallback2);
}

void M2MResourceBase::clear_value()
{
    tr_debug("M2MResourceBase::clear_value");
    sn_nsdl_dynamic_resource_parameters_s* res = get_nsdl_resource();
    free(res->resource);
    res->resource = NULL;
    res->resourcelen = 0;

    report();
}

bool M2MResourceBase::set_value(int64_t value)
{
    bool success;
    // max len of "-9223372036854775808" plus zero termination
    char buffer[20+1];
    uint32_t size = m2m::itoa_c(value, buffer);

    success = set_value((const uint8_t*)buffer, size);

    return success;
}

bool M2MResourceBase::set_value(const uint8_t *value,
                                    const uint32_t value_length)
{
    tr_debug("M2MResourceBase::set_value()");
    bool success = false;
    bool value_changed = false;
    if(is_value_changed(value,value_length)) {
        value_changed = true;
    }
    if( value != NULL && value_length > 0 ) {
        success = true;

        sn_nsdl_dynamic_resource_parameters_s* res = get_nsdl_resource();
        free(res->resource);
        res->resource = NULL;
        res->resourcelen = 0;

        res->resource = alloc_string_copy(value, value_length);
        if(res->resource) {
            res->resourcelen = value_length;
            if( value_changed ) { //
                if (resource_instance_type() == M2MResourceBase::STRING) {
                    M2MReportHandler *report_handler = M2MBase::report_handler();
                    if(report_handler && is_under_observation()) {
                        report_handler->set_notification_trigger();
                    }
                }
                else {
                    report();
                }
            }
        }
    }
    return success;
}

void M2MResourceBase::report()
{
    tr_debug("M2MResourceBase::report()");
    M2MBase::Observation  observation_level = M2MBase::observation_level();
    tr_debug("M2MResourceBase::report() - level %d", observation_level);
    if((M2MBase::O_Attribute & observation_level) == M2MBase::O_Attribute ||
       (M2MBase::OI_Attribute & observation_level) == M2MBase::OI_Attribute) {
        tr_debug("M2MResourceBase::report() -- object/instance level");
        M2MObjectInstance& object_instance = get_parent_resource().get_parent_object_instance();
        object_instance.notification_update(observation_level);
    }

    if(M2MBase::Dynamic == mode() &&
       (M2MBase::R_Attribute & observation_level) == M2MBase::R_Attribute) {
        tr_debug("M2MResourceBase::report() - resource level");
        if(resource_instance_type() != M2MResourceBase::STRING) {
            M2MReportHandler *report_handler = M2MBase::report_handler();
            if (report_handler && is_observable()) {
                sn_nsdl_dynamic_resource_parameters_s* res = get_nsdl_resource();    
                if(res->resource) {
                    report_handler->set_value(atof((const char*)res->resource));
                } else {
                    report_handler->set_value(0);
                }
            }
        }
        else {
            if (base_type() == M2MBase::ResourceInstance) {
                const M2MResource& parent_resource = get_parent_resource();
                M2MReportHandler *report_handler = parent_resource.report_handler();
                if(report_handler && parent_resource.is_observable()) {
                    report_handler->set_notification_trigger(parent_resource.get_parent_object_instance().instance_id());
                }
            }
        }
    } else if(M2MBase::Static == mode()) {
        M2MObservationHandler *observation_handler = M2MBase::observation_handler();
        if(observation_handler) {
            observation_handler->value_updated(this);
        }
    } else {
        tr_debug("M2MResourceBase::report() - mode = %d, is_observable = %d", mode(), is_observable());
    }
}

bool M2MResourceBase::is_value_changed(const uint8_t* value, const uint32_t value_len)
{
    bool changed = false;
    sn_nsdl_dynamic_resource_parameters_s* res = get_nsdl_resource();

    if(value_len != res->resourcelen) {
        changed = true;
    } else if(value && !res->resource) {
        changed = true;
    } else if(res->resource && !value) {
        changed = true;
    } else {
        if (res->resource) {
            if (strcmp((char*)value, (char*)res->resource) != 0) {
                changed = true;
            }
        }
    }
    tr_debug("M2MResourceBase::is_value_changed() -- %s", changed ? "true" : "false");
    return changed;
}

void M2MResourceBase::execute(void *arguments)
{
    // XXX: this line is expected by seven testcases and until this code hits master branch
    // the testcases can not be modified and we need to print the false information too.
    tr_debug("M2MResourceInstance::execute");

    execute_callback* callback = (execute_callback*)M2MCallbackStorage::get_callback(*this, M2MCallbackAssociation::M2MResourceInstanceExecuteCallback);

    if (callback) {
        (*callback)(arguments);
    }

    execute_callback_2 callback2 = (execute_callback_2)M2MCallbackStorage::get_callback(*this, M2MCallbackAssociation::M2MResourceInstanceExecuteCallback2);
    if (callback2) {
        (*callback2)(arguments);
    }
}

void M2MResourceBase::get_value(uint8_t *&value, uint32_t &value_length)
{
    value_length = 0;
    if(value) {
        free(value);
        value = NULL;
    }
    sn_nsdl_dynamic_resource_parameters_s* res = get_nsdl_resource();
    if(res->resource && res->resourcelen > 0) {
        value = alloc_string_copy(res->resource, res->resourcelen);
        if(value) {
            value_length = res->resourcelen;
        }
    }
}

int M2MResourceBase::get_value_int()
{
    int value_int = 0;
    // Get the value and convert it into integer. This is not the most
    // efficient way, as it takes pointless heap copy to get the zero termination.
    uint8_t* buffer = NULL;
    uint32_t length;
    get_value(buffer,length);
    if(buffer) {
        value_int = atoi((const char*)buffer);
        free(buffer);
    }
    return value_int;
}

String M2MResourceBase::get_value_string() const
{
    // XXX: do a better constructor to avoid pointless malloc
    String value;
    if (get_nsdl_resource()->resource) {
        value.append_raw((char*)get_nsdl_resource()->resource, get_nsdl_resource()->resourcelen);
    }
    return value;
}

uint8_t* M2MResourceBase::value() const
{
    return get_nsdl_resource()->resource;
}

uint32_t M2MResourceBase::value_length() const
{   
    return get_nsdl_resource()->resourcelen;
}



#ifndef DISABLE_BLOCK_MESSAGE

M2MBlockMessage* M2MResourceBase::block_message() const
{
    return _block_message_data;
}

bool M2MResourceBase::set_incoming_block_message_callback(incoming_block_message_callback callback)
{
    incoming_block_message_callback* old_callback = (incoming_block_message_callback*)M2MCallbackStorage::remove_callback(*this,
                                                        M2MCallbackAssociation::M2MResourceInstanceIncomingBlockMessageCallback);
    delete old_callback;

    // copy the callback object. This will change on next version to be a direct pointer to a interface class,
    // this FPn<> is just too heavy for this usage.
    incoming_block_message_callback* new_callback = new incoming_block_message_callback(callback);

    delete _block_message_data;
    _block_message_data = NULL;
    _block_message_data = new M2MBlockMessage();

    return M2MCallbackStorage::add_callback(*this, new_callback, M2MCallbackAssociation::M2MResourceInstanceIncomingBlockMessageCallback);
}

bool M2MResourceBase::set_outgoing_block_message_callback(outgoing_block_message_callback callback)
{
    outgoing_block_message_callback *old_callback = (outgoing_block_message_callback*)M2MCallbackStorage::remove_callback(*this,
                                                         M2MCallbackAssociation::M2MResourceInstanceOutgoingBlockMessageCallback);
    delete old_callback;

    outgoing_block_message_callback *new_callback = new outgoing_block_message_callback(callback);
    return M2MCallbackStorage::add_callback(*this, new_callback, M2MCallbackAssociation::M2MResourceInstanceOutgoingBlockMessageCallback);
}
#endif

bool M2MResourceBase::set_notification_sent_callback(notification_sent_callback callback)
{
    notification_sent_callback *old_callback = (notification_sent_callback*)M2MCallbackStorage::remove_callback(*this,
                                                         M2MCallbackAssociation::M2MResourceInstanceNotificationSentCallback);
    delete old_callback;

    notification_sent_callback *new_callback = new notification_sent_callback(callback);
    return M2MCallbackStorage::add_callback(*this, new_callback, M2MCallbackAssociation::M2MResourceInstanceNotificationSentCallback);
}

bool M2MResourceBase::set_notification_sent_callback(notification_sent_callback_2 callback)
{
    M2MCallbackStorage::remove_callback(*this, M2MCallbackAssociation::M2MResourceInstanceNotificationSentCallback2);

    return M2MCallbackStorage::add_callback(*this, (void*)callback, M2MCallbackAssociation::M2MResourceInstanceNotificationSentCallback2);
}

void M2MResourceBase::notification_sent()
{
    // Now we will call both callbacks, if they are set. This is different from original behavior.
    notification_sent_callback* callback = (notification_sent_callback*)M2MCallbackStorage::get_callback(*this, M2MCallbackAssociation::M2MResourceInstanceNotificationSentCallback);
    if (callback) {
        (*callback)();
    }

    notification_sent_callback_2 callback2 = (notification_sent_callback_2)M2MCallbackStorage::get_callback(*this, M2MCallbackAssociation::M2MResourceInstanceNotificationSentCallback2);
    if (callback2) {
        (*callback2)();
    }
}



M2MResourceBase::ResourceType M2MResourceBase::convert_data_type(M2MBase::DataType type) const
{
    M2MResourceBase::ResourceType res_type = M2MResourceBase::OBJLINK;
    switch(type) {
        case M2MBase::STRING:
            res_type = M2MResourceBase::STRING;
            break;
        case M2MBase::INTEGER:
            res_type = M2MResourceBase::INTEGER;
            break;
        case M2MBase::FLOAT:
            res_type = M2MResourceBase::FLOAT;
            break;
        case M2MBase::OPAQUE:
            res_type = M2MResourceBase::OPAQUE;
            break;
        case M2MBase::BOOLEAN:
            res_type = M2MResourceBase::BOOLEAN;
            break;
        case M2MBase::TIME:
            res_type = M2MResourceBase::TIME;
            break;
        case M2MBase::OBJLINK:
            res_type = M2MResourceBase::OBJLINK;
            break;
    }
    return res_type;
}
#endif
