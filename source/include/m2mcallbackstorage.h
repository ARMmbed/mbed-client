/*
 * Copyright (c) 2017 ARM Limited. All rights reserved.
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
#ifndef __M2M_CALLBACK_STORAGE_H__
#define __M2M_CALLBACK_STORAGE_H__

#include "mbed-client/m2mvector.h"

class M2MBase;
class M2MCallbackAssociation;
class M2MCallbackStorage;

typedef m2m::Vector<M2MCallbackAssociation> M2MCallbackAssociationList;

// XXX: this should not be visible for client code
class M2MCallbackAssociation
{
public:

    // Types of callbacks stored as "void*" in the _callback variable.
    // Note: the FPn<> -types are actually stored as pointers to object instances,
    // which needs to be handled externally when fetching and deleting the callbacks.
    enum M2MCallbackType {
        // typedef FP1<void, const char*> value_updated_callback;
        M2MBaseValueUpdatedCallback,

        // typedef void(*value_updated_callback2) (const char* object_name);
        M2MBaseValueUpdatedCallback2,

        // typedef FP1<void,void*> execute_callback;
        M2MResourceInstanceExecuteCallback,

        //typedef void(*execute_callback_2) (void *arguments);
        M2MResourceInstanceExecuteCallback2,

        // typedef FP1<void, M2MBlockMessage *> incoming_block_message_callback;
        M2MResourceInstanceIncomingBlockMessageCallback,

        // typedef FP3<void, const String &, uint8_t *&, uint32_t &> outgoing_block_message_callback;
        M2MResourceInstanceOutgoingBlockMessageCallback,

        // class M2MResourceCallback
        M2MResourceInstanceM2MResourceCallback,

        // typedef FP0<void> notification_sent_callback;
        M2MResourceInstanceNotificationSentCallback,

        // typedef void(*notification_sent_callback_2) (void);
        M2MResourceInstanceNotificationSentCallback2,

    };

    /**
     * Dummy constructor which does not initialize any predictable value to members,
     * needed for array instantiation. Please use the parameterized constructor for real work.
     */
    M2MCallbackAssociation();

    M2MCallbackAssociation(const M2MBase *object, void *callback, M2MCallbackType type);

public:
    /** Object, where the callback is associated to. This is used as key on searches, must not be null. */
    const M2MBase *_object;

    /**
     * Callback pointer, actual data type of the is dependent of _type.
     *
     * We could also use eg. a union with specific pointer types to avoid ugly casts, but that would
     * then require a type-specific get_callback_<type>() to avoid casts on caller side too - unless
     * the get_callback() would return this object and the caller would access the union._callback_<type>
     * variable according to the _type.
     */
    void *_callback;

    /** Type of the data _callback points to and needed for interpreting the data in it. */
    M2MCallbackType _type;
};


class M2MCallbackStorage
{
public:

    ~M2MCallbackStorage();

    // get the shared instance of the storage.
    // TODO: tie this' instance to the nsdlinterface or similar class instead, as it would help
    // to manage the lifecycle of the object. This would also remove the need for these static methods.
    static M2MCallbackStorage *get_instance();

    // utility for deleting the singleton instance, used on unit test and on application exit
    static void delete_instance();

    static bool add_callback(const M2MBase &object, void *callback, M2MCallbackAssociation::M2MCallbackType type);

    // remove all callbacks attached for given object
    static void remove_callbacks(const M2MBase &object);

    // remove callback if one exists, return old value of it
    static void* remove_callback(const M2MBase &object, M2MCallbackAssociation::M2MCallbackType type);

    // XXX: needed for protecting the vector during iteration, but that would add a dependency on PAL here
    // void lock();
    // void release();

    // XXX: const_iterator would be nicer
    //int get_callback(const M2MBase &object, void &*callback, M2MCallbackType type);

    static bool does_callback_exist(const M2MBase &object, M2MCallbackAssociation::M2MCallbackType type);

    static void* get_callback(const M2MBase &object, M2MCallbackAssociation::M2MCallbackType type);

    // XXX: this is actually not needed unless the client has multiple callbacks per object and type.
    inline const M2MCallbackAssociationList& get_callbacks() const;

private:
    bool does_callback_exist(const M2MBase &object, void *callback, M2MCallbackAssociation::M2MCallbackType type) const;

    void do_remove_callbacks(const M2MBase &object);
    void* do_remove_callback(const M2MBase &object, M2MCallbackAssociation::M2MCallbackType type);
    bool do_add_callback(const M2MBase &object, void *callback, M2MCallbackAssociation::M2MCallbackType type);
    void* do_get_callback(const M2MBase &object, M2MCallbackAssociation::M2MCallbackType type) const;

private:

    /**
     * Currently there is only one storage object, which is shared between all the interfaces and
     * M2M objects.
     */
    static M2MCallbackStorage *_static_instance;

    /**
     * Callback objects stored. There combination of <object>+<type> is used on searches and
     * the code does not fully support for adding multiple callbacks for <object>+<type> -pair.
     * But that should not be too hard if the feature was added to M2M object API too, it just
     * requires the M2M objects to iterate through the callbacks associated to it, not just call
     * the get_callback(<object>,<type>) and call the first one.
     */
    M2MCallbackAssociationList _callbacks;
};

inline const M2MCallbackAssociationList& M2MCallbackStorage::get_callbacks() const
{
    return _callbacks;
}

#endif // !__M2M_CALLBACK_STORAGE_H__
