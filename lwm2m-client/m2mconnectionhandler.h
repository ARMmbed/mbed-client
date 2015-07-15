/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_CONNECTION_HANDLER_H__
#define M2M_CONNECTION_HANDLER_H__

#include "lwm2m-client/m2mconnectionobserver.h"
#include "lwm2m-client/m2mconfig.h"
#include "lwm2m-client/m2minterface.h"

//TODO: refactor this to include lwm2m-client header!
#include "sn_nsdl.h"

class M2MConnectionSecurity;
class M2MConnectionHandlerPimpl;

/**
 * @brief M2MConnectionHandler.
 * This class handles the socket connection for LWM2M Client
 */

class M2MConnectionHandler {
public:
    typedef enum {
        CONNECTION_ERROR_WANTS_READ = -1000,
        CONNECTION_ERROR_WANTS_WRITE = -1001
    }ConnectionError;

public:

    /**
    * @brief Constructor
    */
    M2MConnectionHandler(M2MConnectionObserver &observer,
                         M2MConnectionSecurity* sec,
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
                                M2MConnectionObserver::ServerType server_type,
                                const M2MSecurity* security);

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
    bool start_listening_for_data();

    /**
    * @brief Stops listening for incoming data
    */
    void stop_listening();

    /**
     * @brief sendToSocket Sends directly to socket. This is used by
     * security classes to send after data has been encrypted.
     * @param buf Buffer to send
     * @param len Length of a buffer
     * @return Number of bytes sent or -1 if failed
     */
    int sendToSocket(const unsigned char *buf, size_t len);

    /**
     * @brief receiveFromSocket Receives directly from a socket. This
     * is used by security classes to receive raw data to be decrypted.
     * @param buf Buffer to send
     * @param len Length of a buffer
     * @return Number of bytes read or -1 if failed.
     */
    int receiveFromSocket(unsigned char *buf, size_t len);


    /**
    * @brief Closes the open connection
    */
    void close_connection();

private:

    M2MConnectionObserver                       &_observer;
    M2MConnectionHandlerPimpl                   *_private_impl;

friend class Test_M2MConnectionHandler;
friend class Test_M2MConnectionHandler_mbed;
friend class Test_M2MConnectionHandler_linux;
friend class M2MConnection_TestObserver;
};

#endif //M2M_CONNECTION_HANDLER_H__

