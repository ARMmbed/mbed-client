/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_CONNECTION_HANDLER_LINUX_H__
#define M2M_CONNECTION_HANDLER_LINUX_H__

#include "lwm2m-client/m2mconfig.h"
#include "lwm2m-client/m2minterface.h"
#include "m2mconnectionobserver.h"
#include "nsdllinker.h"

#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <signal.h> /* For SIGIGN and SIGINT */
#include <unistd.h>
#include <errno.h>


/**
 * @brief M2MConnectionHandler.
 * This class handles the socket connection for LWM2M Client
 */

class M2MConnectionHandlerImpl {

public:

    /**
    * @brief Constructor
    */
    M2MConnectionHandlerImpl(M2MConnectionObserver &observer,
                             M2MInterface::NetworkStack stack);

    /**
    * @brief Destructor
    */
    ~M2MConnectionHandlerImpl();

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
    * @brief Returns server port which is set.
    * @return Port,Server port
    */
    uint16_t server_port();

    /**
    * @brief Returns listening port which is set.
    * @return Port,Listening port
    */
    uint16_t listen_port();

    /**
    * @brief Listens for incoming data from remote server
    * @return true if successful else false.
    */
    bool listen_for_data();

    void data_receive();

private:

    /**
    * @brief Helper function to return NetworkStack for the stack type
    * @return NetworkStack, NetworkStack type.
    */
    M2MInterface::NetworkStack network_stack();

private:

    M2MConnectionObserver                   &_observer;
    String                                  _server_address;
    char                                    _receive_buffer[1024];
    char                                    _send_buffer[1024];
    M2MInterface::NetworkStack              _stack;
    int                                     _socket_server;
    struct sockaddr_in                      _sa_dst;
    struct sockaddr_in                      _sa_src;
    int                                     _slen_sa_dst;
    M2MConnectionObserver::SocketAddress    *_received_packet_address;
    uint8_t                                 _received_address[4];
    uint8_t                                 _received_buffer[1024];
    uint8_t                                 _resolved_address[16];
    pthread_t                               _listen_thread; /* Thread for Listen data function */


friend class Test_M2MConnectionHandler;

};
#endif //M2M_CONNECTION_HANDLER_LINUX_H__

