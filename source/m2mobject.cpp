/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "lwm2m-client/m2mobject.h"
#include "lwm2m-client/m2mobjectinstance.h"

M2MObject::M2MObject(const String &object_name)
: M2MBase(object_name,M2MBase::Dynamic),
  _instance_index(0)
{
    M2MBase::set_base_type(M2MBase::Object);
}

M2MObject::~M2MObject()
{
    if(!_instance_list.empty()) {
        M2MObjectInstanceList::const_iterator it;
        it = _instance_list.begin();
        M2MObjectInstance* obj = NULL;
        uint16_t index = 0;
        for (; it!=_instance_list.end(); it++, index++ ) {
            //Free allocated memory for object instances.
            obj = *it;

            char obj_inst_id[10];
            sprintf(obj_inst_id,"%d",index);

            String obj_name = name();
            obj_name += String("/");
            obj_name += String(obj_inst_id);

            remove_resource_from_coap(obj_name);

            delete obj;
            obj = NULL;
        }
        remove_object_from_coap();
        _instance_list.clear();
    }    
}

M2MObject& M2MObject::operator=(const M2MObject& other)
{
    if (this != &other) { // protect against invalid self-assignment
        if(!other._instance_list.empty()){
            M2MObjectInstance* ins = NULL;
            M2MObjectInstanceList::const_iterator it;
            it = other._instance_list.begin();
            for (; it!=other._instance_list.end(); it++ ) {
                ins = *it;
                _instance_list.push_back(new M2MObjectInstance(*ins));
            }
        }
        _instance_index = other._instance_index;        
    }
    return *this;
}

M2MObject::M2MObject(const M2MObject& other)
: M2MBase(other)
{
    *this = other;
}

M2MObjectInstance* M2MObject::create_object_instance()
{
    M2MObjectInstance *object_instance = new M2MObjectInstance(this->name());

    add_object_instance(object_instance);
    return object_instance;
}

bool M2MObject::remove_object_instance(uint16_t inst_id)
{
    bool success = false;
    if(!_instance_list.empty()) {
        M2MObjectInstance* obj = NULL;
        M2MObjectInstanceList::const_iterator it;
        it = _instance_list.begin();
        int pos = 0;
        for ( ; it != _instance_list.end(); it++, pos++ ) {
            if((*it)->instance_id() == inst_id) {
                // Instance found and deleted.
                obj = *it;

                char obj_inst_id[10];
                sprintf(obj_inst_id,"%d",obj->instance_id());

                String obj_name = name();
                obj_name += String("/");
                obj_name += String(obj_inst_id);

                remove_resource_from_coap(obj_name);

                delete obj;
                obj = NULL;
                _instance_list.erase(pos);
                success = true;
                break;
            }
        }
    }
    return success;
}

M2MObjectInstance* M2MObject::object_instance(uint16_t inst_id) const
{
    M2MObjectInstance *obj = NULL;
    if(!_instance_list.empty()) {
        M2MObjectInstanceList::const_iterator it;
        it = _instance_list.begin();
        for ( ; it != _instance_list.end(); it++ ) {
            if((*it)->instance_id() == inst_id) {
                // Instance found.
                obj = *it;
                break;
            }
        }
    }
    return obj;
}

const M2MObjectInstanceList& M2MObject::instances() const
{
    return _instance_list;
}

uint16_t M2MObject::instance_count() const
{
    return (uint16_t)_instance_list.size();
}

M2MBase::BaseType M2MObject::base_type() const
{
    return M2MBase::base_type();
}

bool M2MObject::handle_observation_attribute(char *&query)
{
    bool success = false;
    if(!_instance_list.empty()) {
        M2MObjectInstanceList::const_iterator it;
        it = _instance_list.begin();
        for ( ; it != _instance_list.end(); it++ ) {
            success = (*it)->handle_observation_attribute(query);
        }
    }
    return success;
}

void M2MObject::add_object_instance(M2MObjectInstance *obj)
{
    if(obj) {
        obj->set_instance_id(_instance_index);
        _instance_list.push_back(obj);
        _instance_index++;
    }
}
