/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_CONNECTION_HANDLER_H__
#define M2M_CONNECTION_HANDLER_H__

#include "lwm2m-client/m2mconfig.h"
#include "lwm2m-client/m2minterface.h"
#include "include/nsdllinker.h"
#include "lwm2m-client/m2mconnectionobserver.h"

class M2MConnectionHandlerImpl;

/**
 * @brief M2MConnectionHandler.
 * This class handles the socket connection for LWM2M Client
 */

class M2MConnectionHandler {
private:
    // Prevents the use of assignment operator by accident.
    M2MConnectionHandler& operator=( const M2MConnectionHandler& /*other*/ );

    // Prevents the use of copy constructor by accident
    M2MConnectionHandler( const M2MConnectionHandler& /*other*/ );

public:

    /**
    * @brief Constructor
    */
    M2MConnectionHandler(M2MConnectionObserver &observer,
                         M2MInterface::NetworkStack stack);

    /**
    * @brief Destructor
    */
    ~M2MConnectionHandler();

    /**
    * @brief This binds the socket connection.
    * @param listen_port Port to listen for incoming connection.
    * @return true if successfulelse false.
    */
    bool bind_connection(const uint16_t listen_port);

    /**
    * @brief This resolves the server address. Output is
    * returned through callback
    * @param String server address.
    * @param uint16_t Server port.
    * @param ServerType, Server Type to be resolved.
    * @return true if address is valid else false.
    */
    bool resolve_server_address(const String& server_address,
                                const uint16_t server_port,
                                M2MConnectionObserver::ServerType server_type);

    /**
    * @brief Sends data, to the connected sent to server.
    * @param data, Data to be sent.
    */
    bool send_data(uint8_t *data_ptr,
                   uint16_t data_len,
                   sn_nsdl_addr_s *address_ptr);

    /**
    * @brief Listens for incoming data from remote server
    * @return true if successful else false.
    */
    bool listen_for_data();

private:

    M2MConnectionObserver               &_observer;
    M2MConnectionHandlerImpl            *_impl;

friend class Test_M2MConnectionHandler;

};
#endif //M2M_CONNECTION_HANDLER_H__

