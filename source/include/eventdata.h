/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
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
    M2MSecurityData()
    :_object(NULL){}
    ~M2MSecurityData() {}
    M2MSecurity  *_object;
};

class ResolvedAddressData : public EventData
{
public:
    ResolvedAddressData()
    :_address(NULL),
    _port(0){}
    ~ResolvedAddressData() {}
    const M2MConnectionObserver::SocketAddress    *_address;
    uint16_t                                       _port;
};

class ReceivedData : public EventData
{
public:
    ReceivedData()
    :_data(NULL),
    _size(0),
    _port(0),
    _address(NULL){}
    ~ReceivedData() {}
    uint8_t                                         *_data;
    uint16_t                                        _size;
    uint16_t                                        _port;
    const M2MConnectionObserver::SocketAddress      *_address;
};

class M2MRegisterData : public EventData
{
public:
    M2MRegisterData()
    :_object(NULL){}
    ~M2MRegisterData() {}
    M2MSecurity     *_object;
    M2MObjectList    _object_list;
};

class M2MUpdateRegisterData : public EventData
{
public:
    M2MUpdateRegisterData()
    :_object(NULL),
    _lifetime(0){}
    ~M2MUpdateRegisterData() {}
    M2MSecurity     *_object;
    uint32_t        _lifetime;
};


#endif //EVENT_DATA_H

