#ifndef EVENT_DATA_H
#define EVENT_DATA_H

#include "lwm2m-client/m2mvector.h"
#include "m2mconnectionhandler.h"

//FORWARD DECLARATION
class M2MObject;


typedef Vector<M2MObject *> M2MObjectList;

class M2MSecurity;

class EventData
{
public:    
    virtual ~EventData() {}
};

class M2MSecurityData : public EventData
{
public:
    M2MSecurityData() {}
    ~M2MSecurityData() {}
    M2MSecurity  *_object;
};

class ResolvedAddressData : public EventData
{
public:
    ResolvedAddressData() {}
    ~ResolvedAddressData() {}
    const M2MConnectionObserver::SocketAddress    *_address;
    uint16_t                                       _port;
};

class ReceivedData : public EventData
{
public:
    ReceivedData() {}
    ~ReceivedData() {}
    uint8_t                                         *_data;
    uint16_t                                        _size;
    uint16_t                                        _port;
    const M2MConnectionObserver::SocketAddress      *_address;
};

class M2MRegisterData : public EventData
{
public:
    M2MRegisterData() {}
    ~M2MRegisterData() {}
    M2MSecurity     *_object;
    M2MObjectList    _object_list;
};

class M2MUpdateRegisterData : public EventData
{
public:
    M2MUpdateRegisterData() {}
    ~M2MUpdateRegisterData() {}
    uint32_t        _lifetime;
};


#endif //EVENT_DATA_H

