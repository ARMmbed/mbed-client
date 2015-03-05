#include "include/m2mconnectionhandlerimpl_linux.h"
#include "include/nsdlaccesshelper.h"
#include "lwm2m-client/m2mconstants.h"

M2MConnectionHandlerImpl::M2MConnectionHandlerImpl(M2MConnectionObserver &observer,
                                           M2MInterface::NetworkStack stack)
:_observer(observer),
 _stack(stack),
 _slen_sa_dst(sizeof(_sa_dst))
{
    __connection_impl = this;
    _received_packet_address = (M2MConnectionObserver::SocketAddress *)malloc(sizeof(M2MConnectionObserver::SocketAddress));
    memset(_received_packet_address, 0, sizeof(M2MConnectionObserver::SocketAddress));
    _received_packet_address->_address = _received_address;
    _socket_server=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

M2MConnectionHandlerImpl::~M2MConnectionHandlerImpl()
{
    free(_received_packet_address);
}

bool M2MConnectionHandlerImpl::bind_connection(const uint16_t listen_port)
{
    memset((char *) &_sa_src, 0, sizeof(_sa_src));
    _sa_src.sin_family = AF_INET;
    _sa_src.sin_port = htons(listen_port);

    /* Listen to the port */
    _sa_src.sin_addr.s_addr = INADDR_ANY;
    return (bind(_socket_server, (struct sockaddr *) &_sa_src, sizeof(_sa_src))== -1) ? false : true;
}

bool M2MConnectionHandlerImpl::resolve_server_address(const String& server_address,
                                                  const uint16_t server_port,
                                                  M2MConnectionObserver::ServerType server_type)
{
    bool success = true;
    const char* address = server_address.c_str();
    inet_pton(AF_INET, address, &_resolved_address);

    _received_packet_address->_port = ntohs(server_port);
    memcpy(_received_packet_address->_address, _resolved_address, 4);
    _received_packet_address->_stack = _stack;
    _received_packet_address->_length = 4;

    _observer.address_ready(*_received_packet_address,server_type,server_port);
    return success;
}

bool M2MConnectionHandlerImpl::listen_for_data()
{
    bool success = true;
    pthread_create(&_listen_thread, NULL,__data_listen_poll_function, NULL);
    return success;
}

void M2MConnectionHandlerImpl::listen_thread()
{
    int16_t rcv_size=0;
    while (1) {
        usleep(100);
        memset(_received_buffer, 0, 1024);

        char rcv_in_addr[256];
        memset(rcv_in_addr,0,32);
        rcv_size=recvfrom(_socket_server, _received_buffer,
                          1024, 0, (struct sockaddr *)&_sa_dst,
                          (socklen_t*)&_slen_sa_dst);
        if (rcv_size == -1) {
            _observer.socket_error(2);
        } else {
            inet_ntop(AF_INET, &(_sa_dst.sin_addr),rcv_in_addr,INET_ADDRSTRLEN);

            _received_packet_address->_port = ntohs(_sa_dst.sin_port);
            memcpy(_received_packet_address->_address, &_sa_dst.sin_addr, 4);
            _received_packet_address->_stack = _stack;
            _received_packet_address->_length = 4;

            printf("\nReceived %s:%d [%d B] - ", rcv_in_addr, ntohs(_sa_dst.sin_port), rcv_size);
        }

        /* If message received.. */
        if(rcv_size > 0) {
            pthread_join(_listen_thread,NULL);
            _observer.data_available(_received_buffer,rcv_size,*_received_packet_address);
        }
    }
}

bool M2MConnectionHandlerImpl::send_data(uint8_t *data,
                                     uint16_t data_len,
                                     sn_nsdl_addr_s *address)
{
    bool success = true;
    _sa_dst.sin_family = AF_INET;
    _sa_dst.sin_port = htons(address->port);
    memcpy(&_sa_dst.sin_addr, address->addr_ptr, address->addr_len);

    if (sendto(_socket_server, data, data_len, 0, (const struct sockaddr *)&_sa_dst, _slen_sa_dst)==-1) {
        success = false;
        _observer.socket_error(1);
    } else {
        _observer.data_sent();
    }

    return success;
}

M2MInterface::NetworkStack M2MConnectionHandlerImpl::network_stack()
{
    return _stack;
}
