/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#include "include/m2mconnectionhandlerimpl_mbed.h"
#include "include/m2mconnectionhandler.h"
#include "include/nsdlaccesshelper.h"
#include "lwm2m-client/m2mconstants.h"

M2MConnectionHandlerImpl::M2MConnectionHandlerImpl(M2MConnectionObserver &observer,
                                                   M2MInterface::NetworkStack stack)
:_observer(observer),
 _socket_stack(SOCKET_STACK_UNINIT),
 _resolved_Address(new SocketAddr()),
 _resolved(true)
{
    _socket_address = (M2MConnectionObserver::SocketAddress *)malloc(sizeof(M2MConnectionObserver::SocketAddress));
    memset(_socket_address, 0, sizeof(M2MConnectionObserver::SocketAddress));
    _socket_address->_address = _received_address;

    switch(stack) {
        case M2MInterface::Uninitialized:
            _socket_stack = SOCKET_STACK_UNINIT;
            break;
        case M2MInterface::LwIP_IPv4:
            _socket_stack = SOCKET_STACK_LWIP_IPV4;
            break;
        case M2MInterface::LwIP_IPv6:
            _socket_stack = SOCKET_STACK_LWIP_IPV6;
            break;
        case M2MInterface::Reserved:
            _socket_stack = SOCKET_STACK_RESERVED;
            break;
        case M2MInterface::Nanostack_IPv6:
            _socket_stack = SOCKET_STACK_NANOSTACK_IPV6;
            break;
        case M2MInterface::Unknown:
            _socket_stack = SOCKET_STACK_MAX;
            break;
        default:
            break;
    }

    memset(_receive_buffer,0,sizeof(_receive_buffer));

    _socket = new UDPSocket(_socket_stack);

    //TODO: select socket_address_family based on Network stack
    socket_address_family_t socket_family = SOCKET_AF_INET4;
    _socket->open(socket_family);
    _socket->setOnSent(handler_t(this, &M2MConnectionHandlerImpl::send_handler));
    _socket->setOnError(handler_t(this, &M2MConnectionHandlerImpl::error_handler));
}

M2MConnectionHandlerImpl::~M2MConnectionHandlerImpl()
{
    if(_resolved_Address) {
        delete _resolved_Address;
        _resolved_Address = NULL;
    }
    if(_socket) {
        delete _socket;
        _socket = NULL;
    }
    if(_socket_address) {
        free(_socket_address);
    }
}

bool M2MConnectionHandlerImpl::bind_connection(const uint16_t listen_port)
{
    //TODO: Use bind in mbed Socket
    if(_socket) {
        _socket->bind("0.0.0.0", listen_port);
    }
    return true;
}

bool M2MConnectionHandlerImpl::resolve_server_address(const String& server_address,
                                                      const uint16_t server_port,
                                                      M2MConnectionObserver::ServerType server_type)
{
    bool success = true;
    if(_resolved) {
        _resolved = false;
        _server_address = server_address;
        _server_port = server_port;
        _server_type = server_type;

        socket_error_t error = _socket->resolve(_server_address.c_str(),
                                                handler_t(this, &M2MConnectionHandlerImpl::dns_handler));
        if(SOCKET_ERROR_NONE != error) {
            success = false;
        }
    }
    return success;
}

bool M2MConnectionHandlerImpl::listen_for_data()
{
    // Boolean return required for other platforms,
    // not needed in mbed Socket.
    bool success = true;
    _socket->setOnReadable(handler_t(this, &M2MConnectionHandlerImpl::receive_handler));
    return success;
}

bool M2MConnectionHandlerImpl::send_data(uint8_t *data,
                                     uint16_t data_len,
                                     sn_nsdl_addr_s *address)
{
    bool success = true;
    socket_error_t error = _socket->send_to(data, data_len,_resolved_Address,address->port);
    if(error != SOCKET_ERROR_NONE) {
        success =false;
    }
    return success;
}

void M2MConnectionHandlerImpl::send_handler(socket_error_t error)
{
    if(SOCKET_ERROR_NONE == error) {
        _observer.data_sent();
    } else {
        // TODO:Socket error in sending data
        // Define error code.
        _observer.socket_error(3);
    }
}

void M2MConnectionHandlerImpl::receive_handler(socket_error_t error)
{
    memset(_receive_buffer, 0, BUFFER_LENGTH);
    size_t receive_length = sizeof(_receive_buffer);
    SocketAddr remote_address;
    uint16_t remote_port;

    _socket->recv_from(_receive_buffer, &receive_length,&remote_address,&remote_port);
    if (SOCKET_ERROR_NONE == error) {

        //Hold the network_stack temporarily
        M2MInterface::NetworkStack network_stack = _socket_address->_stack;

        memset(_socket_address,0,sizeof(M2MConnectionObserver::SocketAddress));

        _socket_address->_address =remote_address.getImpl();
        //TODO: Current support only for IPv4, add IPv6 support
        _socket_address->_length = 4;
        _socket_address->_port = remote_port;
        _socket_address->_stack = network_stack;

        // Send data for processing.
        _observer.data_available((uint8_t*)_receive_buffer,
                                 receive_length, *_socket_address);
    } else {
        // Socket error in receiving
        _observer.socket_error(1);
    }
}

void M2MConnectionHandlerImpl::dns_handler(socket_error_t error)
{
    _resolved = true;
    if(SOCKET_ERROR_NONE == error) {
        socket_event_t *event = _socket->getEvent();
        memset(_socket_address,0,sizeof(M2MConnectionObserver::SocketAddress));

        _resolved_Address->setAddr(&event->i.d.addr);
        _socket_address->_address =event->i.d.addr.storage;
        //TODO: Current support only for IPv4, add IPv6 support
        _socket_address->_length = 4;
        _socket_address->_stack = get_network_stack(event->i.d.addr.type);
        _socket_address->_port = _server_port;

        _observer.address_ready(*_socket_address,
                                _server_type,
                                _server_port);
    } else {
        //TODO: Socket error in dns resolving,
        // Define error code.
        _observer.socket_error(2);
    }
}

void M2MConnectionHandlerImpl::error_handler(socket_error_t error)
{
    //TODO: Socket error in dns resolving,
    // Define error code.
    if(SOCKET_ERROR_NONE != error) {
        _observer.socket_error(2);
    }
}

M2MInterface::NetworkStack M2MConnectionHandlerImpl::get_network_stack(socket_stack_t stack_type)
{
    M2MInterface::NetworkStack network_stack = M2MInterface::Uninitialized;
    switch(stack_type) {
        case SOCKET_STACK_UNINIT:
            network_stack = M2MInterface::Uninitialized;
            break;
        case SOCKET_STACK_LWIP_IPV4:
            network_stack = M2MInterface::LwIP_IPv4;
            break;
        case SOCKET_STACK_LWIP_IPV6:
            network_stack = M2MInterface::LwIP_IPv6;
            break;
        case SOCKET_STACK_RESERVED:
            network_stack = M2MInterface::Reserved;
            break;
        case SOCKET_STACK_NANOSTACK_IPV6:
            network_stack = M2MInterface::Nanostack_IPv6;
            break;
        case SOCKET_STACK_PICOTCP:
            network_stack = M2MInterface::Nanostack_IPv6;
            break;
        case SOCKET_STACK_MAX:
            network_stack = M2MInterface::Unknown;
            break;
        default:
            break;
    }
    return network_stack;
}

bool M2MConnectionHandlerImpl::event_result(socket_event_t *socket_event)
{
    bool success = true;
    if(socket_event) {
        if(SOCKET_EVENT_ERROR == socket_event->event      ||
           SOCKET_EVENT_RX_ERROR == socket_event->event   ||
           SOCKET_EVENT_TX_ERROR == socket_event->event) {
            success = false;
        }
    }
    return success;
}
