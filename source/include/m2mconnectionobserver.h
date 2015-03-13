#ifndef M2M_CONNECTION_OBSERVER_H__
#define M2M_CONNECTION_OBSERVER_H__

#include "lwm2m-client/m2minterface.h"

/**
 * @brief Observer class for informing socket activity to the state machine
 */

class M2MConnectionObserver
{

public :

    typedef enum {
        Bootstrap,
        LWM2MServer
    }ServerType;

    /**
     * @brief The M2MSocketAddress struct
     * Unified container for holding socket address data
     * across different platforms.
     */
    struct SocketAddress{
        M2MInterface::NetworkStack  _stack;
        void                        *_address;    
        uint8_t                     _length;
        uint16_t                    _port;
    };

    /**
    * @brief Indicates data is available from socket.
    * @param data, data read from socket
    * @param data_size, length of data read from socket.
    * @param address, Server Address from where data is coming.    
    */
    virtual void data_available(uint8_t* data,
                                uint16_t data_size,
                                const M2MConnectionObserver::SocketAddress &address) = 0;

    /**
    * @brief Indicates some error occured in socket.
    * @param error_code, Error code from socket.
    * It cannot be used any further.
    */
    virtual void socket_error(uint8_t error_code) = 0;

    /**
    * @brief Indicates server address resolving is ready.
    * @param address, Resolved socket address.
    * @param server_type, Type of server.
    * @param server_port, Port of the resolved server address.
    */
    virtual void address_ready(const M2MConnectionObserver::SocketAddress &address,
                               M2MConnectionObserver::ServerType server_type,
                               const uint16_t server_port) = 0;

    /**
    * @brief Indicates data has been sent successfully.
    */
    virtual void data_sent() = 0;

};

#endif // M2M_CONNECTION_OBSERVER_H__
