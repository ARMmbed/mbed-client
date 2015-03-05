#include "lwm2m-client/m2mobjectinstance.h"
#include "lwm2m-client/m2mresource.h"

M2MObjectInstance& M2MObjectInstance::operator=(const M2MObjectInstance& other)
{
    if (this != &other) { // protect against invalid self-assignment
        if(!other._resource_list.empty()){
            M2MResource* ins = NULL;
            M2MResourceList::const_iterator it;
            it = other._resource_list.begin();
            for (; it!=other._resource_list.end(); it++ ) {
                ins = *it;
                _resource_list.push_back(new M2MResource(*ins));
            }
        }
    }
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
    if(!_resource_list.empty()) {
        M2MResource* res = NULL;
        M2MResourceList::const_iterator it;
        it = _resource_list.begin();
        for (; it!=_resource_list.end(); it++ ) {
            //Free allocated memory for resources.
            res = *it;
            delete res;
            res = NULL;
        }
        _resource_list.clear();
    }
}

M2MResource* M2MObjectInstance::create_static_resource(const String &resource_name,
                                               const String &resource_type,
                                               const uint8_t *value,
                                               const uint8_t value_length)
{
    M2MResource *resource = new M2MResource(resource_name, resource_type,
                                            M2MResource::Static);
    resource->set_operation(M2MBase::GET_ALLOWED);
    resource->set_observable(false);

    // If assigning memory to value failed then delete the resource object
    // and return NULL.
    if(!resource->set_value(value,value_length)) {
        delete resource;
        resource = NULL;
    }
    add_resource(resource);
    return resource;
}

M2MResource* M2MObjectInstance::create_dynamic_resource(const String &resource_name,
                                                const String &resource_type,
                                                bool observable)
{
    M2MResource *resource = new M2MResource(resource_name, resource_type,
                                            M2MResource::Dynamic);
    resource->set_operation(M2MBase::GET_PUT_ALLOWED);
    resource->set_observable(observable);
    add_resource(resource);
    return resource;
}

bool M2MObjectInstance::remove_resource(const String &resource_name, uint16_t inst_id)
{
    bool success = false;
    if(!_resource_list.empty()) {
        M2MResource* res = NULL;
        M2MResourceList::const_iterator it;
        it = _resource_list.begin();
        res = *it;
        int pos = 0;
        for ( ; it != _resource_list.end(); it++, pos++ ) {
            if(((*it)->name().compare(0,resource_name.size(),resource_name) == 0 &&
              (*it)->instance_id() == inst_id)) {
                // Resource found and deleted.
                //res = *it;
                delete res;
                res = NULL;
                _resource_list.erase(pos);
                success = true;
                break;
            }
        }
    }
    return success;
}

M2MResource* M2MObjectInstance::resource(const String &resource_name, uint16_t inst_id) const
{
    M2MResource *res = NULL;
    if(!_resource_list.empty()) {
        M2MResourceList::const_iterator it;
        it = _resource_list.begin();
        for ( ; it != _resource_list.end(); it++ ) {
            if(((*it)->name().compare(0,resource_name.size(),resource_name) == 0 &&
              (*it)->instance_id() == inst_id)) {
                // Resource found.
                res = *it;
                break;
            }
        }
    }
    return res;
}

const M2MResourceList& M2MObjectInstance::resources() const
{
    return _resource_list;
}

uint16_t M2MObjectInstance::resource_count() const
{
    return (uint16_t)_resource_list.size();
}

uint16_t M2MObjectInstance::resource_count(const String& resource) const
{
    uint16_t count = 0;
    if(!_resource_list.empty()) {
        M2MResourceList::const_iterator it;
        it = _resource_list.begin();
        for ( ; it != _resource_list.end(); it++ ) {
            if(((*it)->name().compare(0,resource.size(),resource) == 0 )) {
                count++;
            }
        }
    }
    return count;
}

M2MBase::BaseType M2MObjectInstance::base_type() const
{
    return M2MBase::ObjectInstance;
}

void M2MObjectInstance::add_resource(M2MResource *res)
{
    if(res) {
        if(!_resource_list.empty()) {
            M2MResourceList::const_iterator it;
            it = _resource_list.begin();
            for ( ; it != _resource_list.end(); ++it ) {
                if((*it)->name().compare(0,res->name().size(),res->name()) == 0) {
                    // Resource with same name exists,
                    // this is a new instance of resource.
                    // increment instance ID.
                    res->set_instance_id((*it)->instance_id() + 1);
                    break;
                }
            }
        }
        _resource_list.push_back(res);
    }
}
