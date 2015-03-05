#ifndef M2M_CONNECTION_HANDLER_IMPL_MBED_H__
#define M2M_CONNECTION_HANDLER_IMPL_MBED_H__

#ifndef USE_LINUX

#include <UDPaSocket.h>
#include "lwm2m-client/m2mconfig.h"
#include "lwm2m-client/m2minterface.h"
#include "include/nsdllinker.h"
#include "include/m2mconnectionobserver.h"

/**
 * @brief M2MConnectionHandlerImpl.
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
    * @brief Listens for incoming data from remote server
    * @return true if successful else false.
    */
    bool listen_for_data();

private:

    /**
    * @brief Callback handler for socket object creation
    */
    void default_handler(void* arg);

    /**
    * @brief Callback handler for sending data over socket.
    */
    void send_handler(void *arg);

    /**
    * @brief Callback handler for receiving data over socket.
    */
    void receive_handler(void *arg);

    /**
    * @brief Callback handler for dns resolving of address
    */
    void dns_handler(void *arg);

    /**
    * @brief Helper function to return NetworkStack for the stack type
    * @param stack_type, Internal Stack type for Socket API.
    * @return NetworkStack, NetworkStack type.
    */
    M2MInterface::NetworkStack get_network_stack(socket_stack_t stack_type);

    /**
    * @brief Returns result of the event generated from socket callbacks.
    * @param socket_event, Socket event generated from socket callbacks.
    * @return true if success else false if some error occured.
    */
    bool event_result(socket_event_t *socket_event);

private:

    M2MConnectionObserver                       &_observer;
    UDPaSocket                                  *_socket;            //owned
    socket_stack_t                              _socket_stack;
    String                                      _server_address;
    char                                        _receive_buffer[1024];
    char                                        _send_buffer[1024];
    CThunk<M2MConnectionHandlerImpl>            _default_irq;
    CThunk<M2MConnectionHandlerImpl>            _recv_irq;
    CThunk<M2MConnectionHandlerImpl>            _send_irq;
    CThunk<M2MConnectionHandlerImpl>            _dns_irq;
    socket_allocator_t                          _socket_alloc;
    SocketBuffer                                _buffer_received;
    M2MConnectionObserver::SocketAddress        *_socket_address;
    M2MConnectionObserver::ServerType           _server_type;
    uint16_t                                    _server_port;
    bool                                        _resolved;

friend class Test_M2MConnectionHandler;

};
#endif

#endif //M2M_CONNECTION_HANDLER_IMPL_MBED_H__

