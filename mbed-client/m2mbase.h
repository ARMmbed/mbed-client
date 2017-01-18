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
#ifndef M2M_BASE_H
#define M2M_BASE_H

// Support for std args
#include <stdint.h>
#include "mbed-client/m2mconfig.h"
#include "mbed-client/m2mreportobserver.h"
#include "mbed-client/functionpointer.h"
#include "mbed-client/m2mstringbuffer.h"
#include "nsdl-c/sn_nsdl.h"
#include "nsdl-c/sn_coap_header.h"
#include "nsdl-c/sn_nsdl_lib.h"

//FORWARD DECLARATION
struct sn_coap_hdr_;
typedef sn_coap_hdr_ sn_coap_hdr_s;
struct nsdl_s;
struct sn_nsdl_addr_;
typedef sn_nsdl_addr_ sn_nsdl_addr_s;

typedef FP1<void, const char*> value_updated_callback;
typedef void(*value_updated_callback2) (const char* object_name);
class M2MObservationHandler;
class M2MReportHandler;

class M2MObjectInstance;
class M2MObject;
class M2MResource;


/*! \file m2mbase.h
 *  \brief M2MBase.
 *  This class is the base class based on which all LWM2M object models
 *  can be created. This serves base class for Object, ObjectInstances and Resources.
 */
class M2MBase : public M2MReportObserver {

public:

    /**
      * Enum to define the type of object.
      */
    typedef enum {
        Object = 0x0,
        Resource = 0x1,
        ObjectInstance = 0x2,
        ResourceInstance = 0x3
    } BaseType;

    /**
      * Enum to define observation level.
      */
    typedef enum {
        None                 = 0x0,
        R_Attribute          = 0x01,
        OI_Attribute         = 0x02,
        OIR_Attribute        = 0x03,
        O_Attribute          = 0x04,
        OR_Attribute         = 0x05,
        OOI_Attribute        = 0x06,
        OOIR_Attribute       = 0x07
    } Observation;


    /**
     * \brief Enum defining a resource type.
    */
    typedef enum {
        Static,
        Dynamic,
        Directory
    }Mode;

    /**
     * Enum defining an operation that can be
     * supported by a given resource.
    */
    typedef enum {
        NOT_ALLOWED                 = 0x00,
        GET_ALLOWED                 = 0x01,
        PUT_ALLOWED                 = 0x02,
        GET_PUT_ALLOWED             = 0x03,
        POST_ALLOWED                = 0x04,
        GET_POST_ALLOWED            = 0x05,
        PUT_POST_ALLOWED            = 0x06,
        GET_PUT_POST_ALLOWED        = 0x07,
        DELETE_ALLOWED              = 0x08,
        GET_DELETE_ALLOWED          = 0x09,
        PUT_DELETE_ALLOWED          = 0x0A,
        GET_PUT_DELETE_ALLOWED      = 0x0B,
        POST_DELETE_ALLOWED         = 0x0C,
        GET_POST_DELETE_ALLOWED     = 0x0D,
        PUT_POST_DELETE_ALLOWED     = 0x0E,
        GET_PUT_POST_DELETE_ALLOWED = 0x0F,
    }Operation;

    enum MaxPathSize {
        MAX_NAME_SIZE = 64,
        MAX_INSTANCE_SIZE = 5,

        MAX_PATH_SIZE = ((MAX_NAME_SIZE * 2) + (MAX_INSTANCE_SIZE * 2) + 3 + 1),
        MAX_PATH_SIZE_2 = ((MAX_NAME_SIZE * 2) + MAX_INSTANCE_SIZE + 2 + 1),
        MAX_PATH_SIZE_3 = (MAX_NAME_SIZE + (MAX_INSTANCE_SIZE * 2) + 2 + 1),
        MAX_PATH_SIZE_4 = (MAX_NAME_SIZE + MAX_INSTANCE_SIZE + 1 + 1),
    };

    typedef struct lwm2m_parameters {
        //add multiple_instances
        uint32_t            max_age;
        uint32_t            instance_id;
        int32_t             name_id;
        char*               name; //for backwards compability
        sn_nsdl_dynamic_resource_parameters_s *dynamic_resource_params;
        BaseType            base_type;
        bool                free_on_delete;   /**< true if struct is dynamically allocted and it
                                                 and its members (name) are to be freed on destructor.
                                                 Note: the sn_nsdl_dynamic_resource_parameters_s has
                                                 its own similar, independent flag.
                                                 Note: this also serves as a read-only flag. */
    } lwm2m_parameters_s;

protected:

    // Prevents the use of default constructor.
    M2MBase();

    // Prevents the use of assignment operator.
    M2MBase& operator=( const M2MBase& /*other*/ );

    // Prevents the use of copy constructor
    M2MBase( const M2MBase& /*other*/ );

    /**
     * \brief Constructor
     * \param name Name of the object created.
     * \param mode Type of the resource.
     * \param resource_type Textual information of resource.
     * \param path Path of the object like 3/0/1
     * \param external_blockwise_store If true CoAP blocks are passed to application through callbacks
     *        otherwise handled in mbed-client-c.
     */
    M2MBase(const String &name,
            M2MBase::Mode mode,
            const String &resource_type,
            char *path,
            bool external_blockwise_store);

    M2MBase(const lwm2m_parameters_s* s);

public:

    /**
     * Destructor
     */
    virtual ~M2MBase();

    /**
     * \brief Sets the operation type for an object.
     * \param operation The operation to be set.
     */
    virtual void set_operation(M2MBase::Operation operation);

#ifndef MEMORY_OPTIMIZED_API
    /**
     * \brief Sets the interface description of the object.
     * \param description The description to be set.
     */
    virtual void set_interface_description(const String &description);

    /**
     * \brief Sets the interface description of the object.
     * \param description The description to be set.
     */
    virtual void set_interface_description(const char *description);

    /**
     * \brief Sets the resource type of the object.
     * \param resource_type The resource type to be set.
     */
    virtual void set_resource_type(const String &resource_type);

    /**
     * \brief Sets the resource type of the object.
     * \param resource_type The resource type to be set.
     */
    virtual void set_resource_type(const char *resource_type);
#endif

    /**
     * \brief Sets the CoAP content type of the object.
     * \param content_type The content type to be set based on
     * CoAP specifications.
     */
    virtual void set_coap_content_type(const uint8_t content_type);

    /**
     * \brief Sets the observable mode for the object.
     * \param observable A value for the observation.
     */
    virtual void set_observable(bool observable);

    /**
     * \brief Adds the observation level for the object.
     * \param observation_level The level of observation.
     */
    virtual void add_observation_level(M2MBase::Observation observation_level);

    /**
     * \brief Removes the observation level for the object.
     * \param observation_level The level of observation.
     */
    virtual void remove_observation_level(M2MBase::Observation observation_level);

    /**
     * \brief Sets the object under observation.
     * \param observed The value for observation. When true, starts observing. When false, the ongoing observation is cancelled.
     * \param handler A handler object for sending
     * observation callbacks.
     */
    virtual void set_under_observation(bool observed,
                                       M2MObservationHandler *handler);
    /**
     * \brief Returns the Observation Handler object.
     * \return M2MObservationHandler object.
    */
    M2MObservationHandler* observation_handler();

    /**
     * \brief Sets the observation handler
     * \param handler Observation handler
    */
    void set_observation_handler(M2MObservationHandler *handler);

    /**
     * \brief Sets the observation token value.
     * \param token A pointer to the token of the resource.
     * \param length The length of the token pointer.
     */
    virtual void set_observation_token(const uint8_t *token,
                                       const uint8_t length);

    /**
     * \brief Sets the instance ID of the object.
     * \param instance_id The instance ID of the object.
     */
    virtual void set_instance_id(const uint16_t instance_id);

    /**
     * This function is deprecated. An increment of the observation number is done internally.
     * \brief Sets the observation number of the object.
     * \param observation_number The observation number of the object.
     */
    virtual void set_observation_number(const uint16_t observation_number)
        m2m_deprecated;

    /**
     * \brief Sets the max age for the resource value to be cached.
     * \param max_age The max age in seconds.
     */
    virtual void set_max_age(const uint32_t max_age);

    /**
     * \brief Returns the object type.
     * \return The base type of the object.
     */
    virtual M2MBase::BaseType base_type() const;

    /**
     * \brief Returns the operation type of the object.
     * \return The supported operation on the object.
     */
    virtual M2MBase::Operation operation() const;

    /**
     * \brief Returns the object name.
     * \return The name of the object.
     */
    virtual const char* name() const;

    /**
     * \brief Returns the object name in integer.
     * \return The name of the object in integer.
     */
    virtual int32_t name_id() const;

    /**
     * \brief Returns the object's instance ID.
     * \returns The instance ID of the object.
     */
    virtual uint16_t instance_id() const;

    /**
     * \brief Returns the interface description of the object.
     * \return The interface description of the object.
     */
    virtual const char* interface_description() const;

    /**
     * \brief Returns the resource type of the object.
     * \return The resource type of the object.
     */
    virtual const char* resource_type() const;

    /**
     * \brief Returns the path of the object.
     * \return The path of the object (eg. 3/0/1).
     */
    virtual const char* uri_path() const;

    /**
     * \brief Returns the CoAP content type of the object.
     * \return The CoAP content type of the object.
     */
    virtual uint8_t coap_content_type() const;

    /**
     * \brief Returns the observation status of the object.
     * \return True if observable, else false.
     */
    virtual bool is_observable() const;

    /**
     * \brief Returns the observation level of the object.
     * \return The observation level of the object.
     */
    virtual M2MBase::Observation observation_level() const;

    /**
     * \brief Provides the observation token of the object.
     * \param value[OUT] A pointer to the value of the token.
     * \param value_length[OUT] The length of the token pointer.
     */
    virtual void get_observation_token(uint8_t *&token, uint32_t &token_length);

    /**
     * \brief Returns the mode of the resource.
     * \return The mode of the resource.
     */
     virtual Mode mode() const;

    /**
     * \brief Returns the observation number.
     * \return The observation number of the object.
     */
    virtual uint16_t observation_number() const;

    /**
     * \brief Returns the max age for the resource value to be cached.
     * \return The maax age in seconds.
     */
    virtual uint32_t max_age() const;

    /**
     * \brief Parses the received query for the notification
     * attribute.
     * \param query The query that needs to be parsed.
     * \return True if required attributes are present, else false.
     */
    virtual bool handle_observation_attribute(const char *query);

    /**
     * \brief Handles GET request for the registered objects.
     * \param nsdl An NSDL handler for the CoAP library.
     * \param received_coap_header The received CoAP message from the server.
     * \param observation_handler A handler object for sending
     * observation callbacks.
     * \return sn_coap_hdr_s The message that needs to be sent to server.
     */
    virtual sn_coap_hdr_s* handle_get_request(nsdl_s *nsdl,
                                              sn_coap_hdr_s *received_coap_header,
                                              M2MObservationHandler *observation_handler = NULL);
    /**
     * \brief Handles PUT request for the registered objects.
     * \param nsdl An NSDL handler for the CoAP library.
     * \param received_coap_header The received CoAP message from the server.
     * \param observation_handler A handler object for sending
     * observation callbacks.
     * \param execute_value_updated True executes the "value_updated" callback.
     * \return sn_coap_hdr_s The message that needs to be sent to server.
     */
    virtual sn_coap_hdr_s* handle_put_request(nsdl_s *nsdl,
                                              sn_coap_hdr_s *received_coap_header,
                                              M2MObservationHandler *observation_handler,
                                              bool &execute_value_updated);

    /**
     * \brief Handles GET request for the registered objects.
     * \param nsdl An NSDL handler for the CoAP library.
     * \param received_coap_header The received CoAP message from the server.
     * \param observation_handler A handler object for sending
     * observation callbacks.
     * \param execute_value_updated True executes the "value_updated" callback.
     * \return sn_coap_hdr_s  The message that needs to be sent to server.
     */
    virtual sn_coap_hdr_s* handle_post_request(nsdl_s *nsdl,
                                               sn_coap_hdr_s *received_coap_header,
                                               M2MObservationHandler *observation_handler,
                                               bool &execute_value_updated,
                                               sn_nsdl_addr_s *address = NULL);

    /**
     * \brief Sets whether this resource is published to server or not.
     * \param register_uri True sets the resource as part of registration message.
     */
    virtual void set_register_uri(bool register_uri);

    /**
     * \brief Returns whether this resource is published to server or not.
     * \return True if the resource is a part of the registration message, else false.
     */
    virtual bool register_uri();

    /**
     * @brief Returns whether this resource is under observation or not.
     * @return True if the resource is under observation, else false,
     */
    virtual bool is_under_observation() const;

    /**
     * @brief Sets the function that is executed when this
     * object receives a PUT or POST command.
     * @param callback The function pointer that is called.
     */
    virtual void set_value_updated_function(value_updated_callback callback);

    /**
     * @brief Sets the function that is executed when this
     * object receives a PUT or POST command.
     * @param callback The function pointer that is called.
     */
    virtual void set_value_updated_function(value_updated_callback2 callback);

    /**
     * @brief Returns whether a callback function is set or not.
     * @return True if the callback function is set, else false.
     */
    virtual bool is_value_updated_function_set();

    /**
     * @brief Calls the function that is set in the "set_value_updated_function".
     * @param name The name of the object.
     */
    virtual void execute_value_updated(const String& name);

    /**
     * @brief Returns length of the object name.
     * @return Length of the object name.
     */
    size_t resource_name_length() const;

    /**
     * @brief Returns the resource information.
     * @return Resource information.
     */
    sn_nsdl_dynamic_resource_parameters_s* get_nsdl_resource();

    static char* create_path(const M2MObject &parent, const char *name);
    static char* create_path(const M2MObject &parent, uint16_t object_instance);
    static char* create_path(const M2MResource &parent, uint16_t resource_instance);
    static char* create_path(const M2MResource &parent, const char *name);
    static char* create_path(const M2MObjectInstance &parent, const char *name);

protected : // from M2MReportObserver

    virtual void observation_to_be_sent(m2m::Vector<uint16_t> changed_instance_ids,
                                        bool send_object = false);

protected:

    /**
     * \brief Sets the base type for an object.
     * \param type The base type of the object.
     */
    virtual void set_base_type(M2MBase::BaseType type);

    /**
     * \brief Memory allocation required for libCoap.
     * \param size The size of memory to be reserved.
    */
    static void* memory_alloc(uint32_t size);

    /**
     * \brief Memory free functions required for libCoap.
     * \param ptr The object whose memory needs to be freed.
    */
    static void memory_free(void *ptr);

    /**
     * \brief Allocate and make a copy of given zero terminated string. This
     * is functionally equivalent with strdup().
     * \param source The source string to copy, may not be NULL.
    */
    static char* alloc_string_copy(const char* source);

    /**
     * \brief Allocate (size + 1) amount of memory, copy size bytes into
     * it and add zero termination.
     * \param source The source string to copy, may not be NULL.
     * \param size The size of memory to be reserved.
    */
    static uint8_t* alloc_string_copy(const uint8_t* source, uint32_t size);

    /**
     * \brief Allocate (size) amount of memory, copy size bytes into it.
     * \param source The source buffer to copy, may not be NULL.
     * \param size The size of memory to be reserved.
    */
    static uint8_t* alloc_copy(const uint8_t* source, uint32_t size);

    // validate string length to be [min_length..max_length]
    static bool validate_string_length(const String &string, size_t min_length, size_t max_length);
    static bool validate_string_length(const char* string, size_t min_length, size_t max_length);

    /**
     * \brief Create Report Handler object.
     * \return M2MReportHandler object.
    */
    M2MReportHandler* create_report_handler();

    /**
     * \brief Returns the Report Handler object.
     * \return M2MReportHandler object.
    */
    M2MReportHandler* report_handler();


    static bool build_path(StringBuffer<MAX_PATH_SIZE> &buffer, const char *s1, uint16_t i1, const char *s2, uint16_t i2);

    static bool build_path(StringBuffer<MAX_PATH_SIZE_2> &buffer, const char *s1, uint16_t i1, const char *s2);

    static bool build_path(StringBuffer<MAX_PATH_SIZE_3> &buffer, const char *s1, uint16_t i1, uint16_t i2);

    static bool build_path(StringBuffer<MAX_PATH_SIZE_4> &buffer, const char *s1, uint16_t i1);

    static char* stringdup(const char* s);

private:

    static bool is_integer(const String &value);

    static bool is_integer(const char *value);

    void free_resources();

private:
    lwm2m_parameters_s          *_sn_resource;
    M2MReportHandler            *_report_handler;
    M2MObservationHandler       *_observation_handler; // Not owned
    uint8_t                     *_token;
    FP1<void, const char*>      *_function_pointer;
    value_updated_callback      *_value_updated_callback;
    unsigned                    _observation_number : 16;
    unsigned                    _token_length : 8;
    M2MBase::Observation        _observation_level : 4;
    bool                        _is_under_observation : 1;

friend class Test_M2MBase;
friend class Test_M2MObject;

};

#endif // M2M_BASE_H

