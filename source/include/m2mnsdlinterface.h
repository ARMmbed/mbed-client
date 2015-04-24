/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2MNSDLINTERFACE_H
#define M2MNSDLINTERFACE_H

#include "lwm2m-client/m2mvector.h"
#include "lwm2m-client/m2mconfig.h"
#include "nsdllinker.h"
#include "m2mtimer.h"
#include "lwm2m-client/m2mobservationhandler.h"

//FORWARD DECLARARTION
class M2MSecurity;
class M2MObject;
class M2MObjectInstance;
class M2MResource;
class M2MNsdlObserver;
class M2MBase;
class M2MServer;

typedef Vector<M2MObject *> M2MObjectList;
typedef Vector<M2MServer *> M2MServerList;

class M2MNsdlInterface : public M2MTimerObserver,
                         public M2MObservationHandler
{

public:
    /**
    * @brief Constructor
    * @param observer, Observer to pass the event callbacks from nsdl library.
    */
    M2MNsdlInterface(M2MNsdlObserver &observer);

    /**
     * @brief Destructor
     */
    virtual ~M2MNsdlInterface();

    /**
     * @brief Creates endpoint object for the nsdl stack.
     * @param endpoint_name, Endpoint name of the client.
     * @param endpoint_type, Endpoint type of the client.
     * @param life_time, Life time of the client in seconds
     * @param domain, Domain of the client.
     * @param mode, Binding mode of the client, default is UDP
     * @param context_address, Context address default is empty.
    */
    void create_endpoint(const String &endpoint_name,
                         const String &endpoint_type,
                         const int32_t life_time,
                         const String &domain,
                         const uint8_t mode,
                         const String &context_address);

    /**
     * @brief Deletes the endpoint.
    */
    void delete_endpoint();

    /**
     * @brief Creates the NSDL structure for the registered objectlist.
     * @param object_list, List of objects to be registered.
     * @return true if structure created successfully else false.
    */
    bool create_nsdl_list_structure(const M2MObjectList &object_list);

    /**
     * @brief Removed the NSDL resource for the given resource.
     * @param resource_name, Resource name to be removed.
     * @return true if removed successfully else false.
    */
    bool delete_nsdl_resource(const String &resource_name);

    /**
     * @brief Creates the bootstrap object.
     * @param address Bootstrap address.
     * @return true if created and sent successfully else false.
    */
    bool create_bootstrap_resource(sn_nsdl_addr_s *address);

    /**
     * @brief Sends the register message to the server.
     * @param address M2MServer address.
     * @param port M2MServer port.
     * @param address_type IP Address type.
     * @return  true if register sent successfully else false.
    */
    bool send_register_message(uint8_t* address,
                               const uint16_t port,
                               sn_nsdl_addr_type_e address_type);

    /**
     * @brief Sends the update registration message to the server.
     * @param lifetime, Updated lifetime value in seconds.
     * @return  true if sent successfully else false.
     *
    */
    bool send_update_registration(const uint32_t lifetime);

    /**
     * @brief Sends unregister message to the server.
     * @return  true if unregister sent successfully else false.
    */
    bool send_unregister_message();

    /**
     * @brief Memory Allocation required for libCoap.
     * @param size, Size of memory to be reserved.
    */
    void* memory_alloc(uint16_t size);

    /**
     * @brief Memory free functions required for libCoap
     * @param ptr, Object whose memory needs to be freed.
    */
    void memory_free(void *ptr);

    /**
    * @brief Callback from nsdl library to inform the data is ready
    * to be sent to server.
    * @param protocol, Protocol format of the data
    * @param data, Data to be sent.
    * @param data_len, Size of the data to be sent
    * @param address, server address where data has to be sent.
    * @return 1 if successful else 0.
    */
    uint8_t send_to_server_callback(sn_nsdl_capab_e protocol,
                                    uint8_t *data,
                                    uint16_t data_len,
                                    sn_nsdl_addr_s *address);

    /**
    * @brief Callback from nsdl library to inform the data which is
    * received from server for the client has been converted to coap message.
    * @param coap_header, Coap message formed from data.
    * @param address, Server address from where the data is received.
    * @return 1 if successful else 0.
    */
    uint8_t received_from_server_callback(sn_coap_hdr_s *coap_header,
                                          sn_nsdl_addr_s *address);

    /**
    * @brief Callback from nsdl library to inform the data which is
    * received from server for the resources has been converted to coap message.
    * @param coap_header, Coap message formed from data.
    * @param address, Server address from where the data is received.
    * @param proto, Protocol for the message, currently only coap is supported.
    * @return 1 if successful else 0.
    */
    uint8_t resource_callback(sn_coap_hdr_s *coap,
                               sn_nsdl_addr_s *address,
                               sn_proto_info_s * proto);

    void bootstrap_done_callback(sn_nsdl_oma_server_info_t *server_info);

    bool process_received_data(uint8_t *data,
                               uint16_t data_size,
                               sn_nsdl_addr_s *address);

protected: // from M2MTimerObserver

    virtual void timer_expired(M2MTimerObserver::Type type);

protected: // from M2MObservationHandler

    virtual void observation_to_be_sent(M2MBase *object);

    virtual void resource_to_be_deleted(const String &resource_name);

    virtual void remove_object(M2MBase *object);

private:

    /**
    * @brief Initializes all the nsdl library component to be usable.
    * @return true if initialization is successful else false.
    */
    bool initialize();

    /**
     * @brief Adds object to the observation list
     * @param object, Object to be added
     * @return true if added else false if already exists.
     */
    bool add_object_to_list(M2MObject *object);

    bool create_nsdl_object_structure(M2MObject *object);

    bool create_nsdl_object_instance_structure(M2MObjectInstance *object_instance);

    bool create_nsdl_resource_structure(M2MResource *resource,
                                        const String &object_name = "");

    String coap_to_string(uint8_t *coap_data_ptr,
                          int coap_data_ptr_length);

    void execute_nsdl_process_loop();

    uint64_t registration_time();

    M2MBase* find_resource(const String &object);

    M2MBase* find_resource(const M2MObject *object,
                           const String &object_instance);

    M2MBase* find_resource(const M2MObjectInstance *object_instance,
                           const String &resource_instance);

    bool object_present(M2MObject * object) const;

    uint8_t handle_get_request(sn_coap_hdr_s *received_coap_header,
                               M2MBase *object,
                               sn_nsdl_addr_s *address);


    uint8_t handle_put_request(sn_coap_hdr_s *received_coap_header,
                               M2MBase *object,
                               sn_nsdl_addr_s *address);

    uint8_t handle_post_request(sn_coap_hdr_s *received_coap_header,
                               M2MBase *object,
                               sn_nsdl_addr_s *address);

    void clear_resource(sn_nsdl_resource_info_s *&resource);

private:

    M2MNsdlObserver                   &_observer;
    M2MObjectList                      _object_list;
    M2MServerList                      _server_list;
    M2MTimer                          *_nsdl_exceution_timer;
    M2MTimer                          *_registration_timer;
    sn_nsdl_ep_parameters_s           *_endpoint;
    sn_nsdl_resource_info_s           *_resource;
    sn_nsdl_bs_ep_info_t               _bootstrap_endpoint;
    sn_nsdl_oma_device_t               _bootstrap_device_setup;
    sn_nsdl_addr_s                     _sn_nsdl_address;
    uint32_t                           _counter_for_nsdl;

friend class Test_M2MNsdlInterface;

};

#endif // M2MNSDLINTERFACE_H
