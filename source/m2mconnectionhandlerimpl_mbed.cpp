#include "m2mconnectionhandlerimpl_mbed.h"
#include "m2mconnectionhandler.h"
#include "nsdlaccesshelper.h"
#include "m2mconstants.h"

M2MConnectionHandlerImpl::M2MConnectionHandlerImpl(M2MConnectionObserver &observer,
                                                   M2MInterface::NetworkStack stack)
:_observer(observer),
 _socket_stack(SOCKET_STACK_UNINIT),
 _default_irq(this),
 _recv_irq(this),
 _send_irq(this),
 _dns_irq(this),
 _resolved(true)
{
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
    _default_irq.callback(&M2MConnectionHandlerImpl::default_handler);
    _send_irq.callback(&M2MConnectionHandlerImpl::send_handler);
    _recv_irq.callback(&M2MConnectionHandlerImpl::receive_handler);
    _dns_irq.callback(&M2MConnectionHandlerImpl::dns_handler);

    _socket_alloc.alloc = __socket_malloc;    
    _socket_alloc.dealloc = __socket_free;
    _socket_alloc.context = NULL;

    memset(_receive_buffer,0,sizeof(_receive_buffer));
    memset(_send_buffer,0,sizeof(_send_buffer));

    _socket = new UDPaSocket((handler_t)_default_irq.entry(), _socket_stack, &_socket_alloc);
}

M2MConnectionHandlerImpl::~M2MConnectionHandlerImpl()
{
    if(_socket) {
        delete _socket;
        _socket = NULL;
    }
}

bool M2MConnectionHandlerImpl::bind_connection(const uint16_t listen_port)
{
    SocketAddr socket_address;
    return (_socket->bind(&socket_address,listen_port) == SOCKET_ERROR_NONE) ? true : false;
}

bool M2MConnectionHandlerImpl::resolve_server_address(const String& server_address,
                                                      const uint16_t server_port,
                                                      M2MConnectionObserver::ServerType server_type)
{
    bool success = false;
    if(_resolved) {
        _resolved = false;
        _server_address = server_address;
        _server_port = server_port;
        _server_type = server_type;

        socket_error_t error = _socket->resolve(_server_address.c_str(),(handler_t)_dns_irq.entry());
        if(SOCKET_ERROR_UNKNOWN == error   || SOCKET_ERROR_NULL_PTR == error        ||
           SOCKET_ERROR_BAD_FAMILY == error|| SOCKET_ERROR_TIMEOUT == error         ||
           SOCKET_ERROR_BAD_ALLOC == error || SOCKET_ERROR_NO_CONNECTION == error   ||
           SOCKET_ERROR_SIZE == error      || SOCKET_ERROR_BAD_BUFFER == error      ||
           SOCKET_ERROR_BAD_STACK == error || SOCKET_ERROR_BAD_ADDRESS == error     ||
           SOCKET_ERROR_DNS_FAILED == error) {
            success = false;
        }
    }
    return success;
}

bool M2MConnectionHandlerImpl::listen_for_data()
{

    bool success = true;
    handler_t receive_handler = (handler_t)_recv_irq.entry();
    socket_error_t error = _socket->start_recv(receive_handler);
    if(error != SOCKET_ERROR_NONE) {
        success = false;
    }
    return success;
}

bool M2MConnectionHandlerImpl::send_data(uint8_t *data,
                                     uint16_t data_len,
                                     sn_nsdl_addr_s *address)
{
    bool success = true;
    //Clean the send buffer
    memset(_send_buffer, 0, BUFFER_LENGTH);

    //Copy the data to be sent
    memcpy(_send_buffer,data,data_len);
    handler_t send_handler = (handler_t)_send_irq.entry();

    socket_addr addr;
    addr.type = _socket_stack;
    addr.impl = (void*)address->addr_ptr;

    SocketAddr send_address;
    send_address.setAddr(&addr);

    socket_error_t error = _socket->start_send_to(&send_address, address->port,
                                                  _send_buffer, strlen(_send_buffer),
                                                  0, send_handler);
    if(error != SOCKET_ERROR_NONE) {
        success =false;
    }
    return success;
}


void M2MConnectionHandlerImpl::default_handler(void* /*arg*/)
{
}

void M2MConnectionHandlerImpl::send_handler(void */*arg*/)
{
    socket_event_t *event = _socket->getEvent();
    if(event_result(event)) {
        _observer.data_sent();
    } else {
        // Socket error in sending data
        _observer.socket_error(3);
    }
}

void M2MConnectionHandlerImpl::receive_handler(void */*arg*/)
{
    memset(_receive_buffer, 0, BUFFER_LENGTH);
    socket_event_t *event = _socket->getEvent();
    if(event_result(event)) {
        _buffer_received.set(&(event->i.r.buf));
        uint16_t size = _buffer_received.copyOut(_receive_buffer, sizeof(_receive_buffer));

        M2MConnectionObserver::SocketAddress *address =
                (M2MConnectionObserver::SocketAddress*)malloc(sizeof(M2MConnectionObserver::SocketAddress));
        memset(address,0,sizeof(M2MConnectionObserver::SocketAddress));

        address->_address =&event->i.r.src.impl;
        address->_stack = get_network_stack(event->i.r.src.type);
        address->_port = event->i.r.port;

        // Send data for processing.
        _observer.data_available((uint8_t*)_receive_buffer,
                                 size, *address);
        free(address);
    } else {
        // Socket error in receiving
        _observer.socket_error(1);
    }
}

void M2MConnectionHandlerImpl::dns_handler(void */*arg*/)
{
    _resolved = true;
    socket_event_t *event = _socket->getEvent();
    if(event_result(event)) {
        M2MConnectionObserver::SocketAddress *resolved_address =
                (M2MConnectionObserver::SocketAddress*)malloc(sizeof(M2MConnectionObserver::SocketAddress));
        memset(resolved_address,0,sizeof(M2MConnectionObserver::SocketAddress));
        resolved_address->_address =&event->i.d.addr.impl;
        resolved_address->_stack = get_network_stack(event->i.d.addr.type);

        _observer.address_ready(*resolved_address,
                                _server_type,
                                _server_port);
        free(resolved_address);
    } else {
        // Socket error in dns resolving
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
