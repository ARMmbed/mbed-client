/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_INTERFACE_IMPL_H
#define M2M_INTERFACE_IMPL_H

#include "lwm2m-client/m2minterface.h"
#include "lwm2m-client/m2mserver.h"
#include "m2mnsdlobserver.h"
#include "m2mconnectionobserver.h"

//FORWARD DECLARATION
class M2MNsdlInterface;
class M2MConnectionHandler;
class EventData;

/**
 *  @brief M2MInterfaceImpl.
 *  This class implements handling of all LWM2M Interface operations
 *  defined in OMA LWM2M specifications.
 *  This includes Bootstrapping, Client Registration, Device Management &
 *  Service Enablement and Information Reporting.
 */

class  M2MInterfaceImpl : public M2MInterface,
                          public M2MNsdlObserver,
                          public M2MConnectionObserver
{

friend class M2MInterfaceFactory;

private:

    /**
     * @brief Constructor
     * @param observer, Observer to pass the event callbacks for various
     * interface operations.
     * @param endpoint_name, Endpoint name of the client.
     * @param endpoint_type, Endpoint type of the client.
     * @param life_time, Life time of the client in seconds
     * @param listen_port, Listening port for the endpoint, default is 8000.
     * @param domain, Domain of the client.
     * @param mode, Binding mode of the client, default is UDP
     * @param stack, Network Stack to be used for connection, default is LwIP_IPv4
     * @param context_address, Context address default is empty.
     */
    M2MInterfaceImpl(M2MInterfaceObserver& observer,
                     const String &endpoint_name,
                     const String &endpoint_type,
                     const int32_t life_time,
                     const uint16_t listen_port,
                     const String &domain = "",
                     BindingMode mode = M2MInterface::UDP,
                     M2MInterface::NetworkStack stack = M2MInterface::LwIP_IPv4,
                     const String &context_address = "");

public:

    /**
     * @brief Destructor
     */
    virtual ~M2MInterfaceImpl();

    /**
     * @brief Initiates bootstrapping of the client with the provided Bootstrap
     * server information.
     * @param security_object, Security object which contains information
     * required for successful bootstrapping of the client.
     */
    void bootstrap(M2MSecurity *security);

    /**
     * @brief Cancels on going bootstrapping operation of the client. If the client has
     * already successfully bootstrapped then this function deletes existing
     * bootstrap information from the client.
     */
    void cancel_bootstrap();

    /**
     * @brief Initiates registration of the provided Security object to the
     * corresponding LWM2M server.
     * @param security_object, Security object which contains information
     * required for registering to the LWM2M server.
     * If client wants to register to multiple LWM2M servers then it has call
     * this function once for each of LWM2M server object separately.
     * @param object_list, Objects which contains information
     * which the client want to register to the LWM2M server.
     */
    void register_object(M2MSecurity *security_object, const M2MObjectList &object_list);

    /**
     * @brief Updates or refreshes the client's registration on the LWM2M
     * server.
     * @param security_object, Security object from which the device object
     * needs to update registration, if there is only one LWM2M server registered
     * then this parameter can be NULL.
     * @param lifetime, Lifetime for the endpoint client in seconds.
     */
    virtual void update_registration(M2MSecurity *security_object, const uint32_t lifetime);

    /**
     * @brief Unregisters the registered object from the LWM2M server
     * @param security_object, Security object from which the device object
     * needs to be unregistered, if there is only one LWM2M server registered
     * then this parameter can be NULL.
     */
    void unregister_object(M2MSecurity* security = NULL);

protected: // From M2MNsdlObserver

    virtual void coap_message_ready(uint8_t *data_ptr,
                                    uint16_t data_len,
                                    sn_nsdl_addr_s *address_ptr);

    virtual void client_registered(M2MServer *server_object);

    virtual void registration_error(uint8_t error_code);

    virtual void client_unregistered();

    virtual void bootstrap_done(M2MSecurity *security_object);

    virtual void bootstrap_error();

    virtual void coap_data_processed();

protected: // From M2MConnectionObserver

    virtual void data_available(uint8_t* data,
                                uint16_t data_size,
                                const M2MConnectionObserver::SocketAddress &address);

    virtual void socket_error(uint8_t error_code);

    virtual void address_ready(const M2MConnectionObserver::SocketAddress &address,
                               M2MConnectionObserver::ServerType server_type,
                               const uint16_t server_port);

    void data_sent();

private: // state machine state functions

    /**
    * When the state is Idle.
    */
    void state_idle(EventData* data);

    /**
    * When the client starts bootstrap.
    */
    void state_bootstrap( EventData *data);

    /**
    * When the bootstrap server address is resolved.
    */
    void state_bootstrap_address_resolved( EventData *data);

    /**
    * When the bootstrap resource is created.
    */
    void state_bootstrap_resource_created( EventData *data);

    /**
    * When the server has sent response and bootstrapping is done.
    */
    void state_bootstrapped( EventData *data);

    /**
    * When the client starts register.
    */
    void state_register( EventData *data);

    /**
    * When the server address for register is resolved.
    */
    void state_register_address_resolved( EventData *data);

    /**
    * When the register resource is created.
    */
    void state_register_resource_created( EventData *data);

    /**
    * When the client is registered.
    */
    void state_registered( EventData *data);

    /**
    * When the client is updating registration.
    */
    void state_update_registration( EventData *data);

    /**
    * When the client starts unregister.
    */
    void state_unregister( EventData *data);

    /**
    * When the client has been unregistered.
    */
    void state_unregistered( EventData *data);

    /**
    * When the coap data is been sent through socket.
    */
    void state_sending_coap_data( EventData *data);

    /**
    * When the coap data is sent successfully.
    */
    void state_coap_data_sent( EventData *data);

    /**
    * When the socket is receiving coap data.
    */
    void state_receiving_coap_data( EventData *data);

    /**
    * When the socket has received coap data.
    */
    void state_coap_data_received( EventData *data);

    /**
    * When the coap message is being processed.
    */
    void state_processing_coap_data( EventData *data);

    /**
    * When the coap message has been processed.
    */
    void state_coap_data_processed( EventData *data);

    /**
    * When the client is waiting to receive or send data.
    */
    void state_waiting( EventData *data);

    /**
    * State enumeration order must match the order of state
    * method entries in the state map
    */
    enum E_States {
        STATE_IDLE = 0,
        STATE_BOOTSTRAP,
        STATE_BOOTSTRAP_ADDRESS_RESOLVED,
        STATE_BOOTSTRAP_RESOURCE_CREATED,
        STATE_BOOTSTRAPPED,
        STATE_REGISTER,
        STATE_REGISTER_ADDRESS_RESOLVED,
        STATE_REGISTER_RESOURCE_CREATED,
        STATE_REGISTERED,
        STATE_UPDATE_REGISTRATION,
        STATE_UNREGSITER,
        STATE_UNREGSITERED,
        STATE_SENDING_COAP_DATA,
        STATE_COAP_DATA_SENT,
        STATE_RECEIVING_COAP_DATA,
        STATE_COAP_DATA_RECEIVED,
        STATE_PROCESSING_COAP_DATA,
        STATE_COAP_DATA_PROCESSED,
        STATE_WAITING,
        STATE_MAX_STATES
    };

    /**
     * @brief Redirects the state machine to right function.
     * @param current_state Current state to be set.
     * @param data, Data to be passed to the state function.
     */
    void state_function( uint8_t current_state, EventData* data  );

    /**
     * @brief State Engine maintaining state machine logic.
     */
    void state_engine(void);

    /**
    * External event which can trigger the state machine.
    * @param New State which the state machine should go to.
    * @param data to be passed to the state machine.
    */
    void external_event(uint8_t, EventData* = NULL);

    /**
    * Internal event generated by state machine.
    * @param New State which the state machine should go to.
    * @param data to be passed to the state machine.
    */
    void internal_event(uint8_t, EventData* = NULL);

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

    enum
    {
        EVENT_IGNORED = 0xFE,
        CANNOT_HAPPEN
    };

private:

    M2MInterfaceObserver        &_observer;
    M2MConnectionHandler        *_connection_handler;
    M2MNsdlInterface            *_nsdl_interface;
    uint8_t                     _current_state;
    const int                   _max_states;
    bool                        _event_generated;
    EventData*                  _event_data;

    String                      _endpoint_name;
    String                      _endpoint_type;
    String                      _domain;
    int32_t                     _life_time;
    BindingMode                 _binding_mode;
    String                      _context_address;
    uint16_t                    _listen_port;
    M2MSecurity                 *_register_server; //TODO: to be the list not owned

   friend class Test_M2MInterfaceImpl;

};

#define BEGIN_TRANSITION_MAP \
    static const uint8_t TRANSITIONS[] = {\

#define TRANSITION_MAP_ENTRY(entry)\
    entry,

#define END_TRANSITION_MAP(data) \
    0 };\
    external_event(TRANSITIONS[_current_state], data);

#endif //M2M_INTERFACE_IMPL_H


