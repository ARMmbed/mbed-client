/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <assert.h>
#include "include/m2minterfaceimpl.h"
#include "include/eventdata.h"
#include "mbed-client/m2minterfaceobserver.h"
#include "mbed-client/m2mconnectionhandler.h"
#include "mbed-client/m2mconnectionhandlerfactory.h"
#include "include/m2mnsdlinterface.h"
#include "mbed-client/m2msecurity.h"
#include "mbed-client/m2mconstants.h"
#include "ns_trace.h"

M2MInterfaceImpl::M2MInterfaceImpl(M2MInterfaceObserver& observer,
                                   const String &ep_name,
                                   const String &ep_type,
                                   const int32_t l_time,
                                   const uint16_t listen_port,
                                   const String &dmn,
                                   M2MInterface::BindingMode mode,
                                   M2MInterface::NetworkStack stack,
                                   const String &con_addr)
: _observer(observer),
  _nsdl_interface(new M2MNsdlInterface(*this)),
  _current_state(0),
  _max_states( STATE_MAX_STATES ),
  _event_generated(false),
  _event_data(NULL),
  _endpoint_name(ep_name),
  _endpoint_type(ep_type),
  _domain( dmn),
  _life_time(l_time),
  _binding_mode(mode),
  _context_address(con_addr),
  _listen_port(listen_port),
  _register_server(NULL),
  _event_ignored(false),
  _register_ongoing(false),
  _update_register_ongoing(false)
{
    //Hack for now
    if( _binding_mode == M2MInterface::TCP ){
        _binding_mode = M2MInterface::UDP;
    }else if( _binding_mode == M2MInterface::TCP_QUEUE ){
        _binding_mode = M2MInterface::UDP_QUEUE;
    }
    tr_debug("M2MInterfaceImpl::M2MInterfaceImpl() -IN");
    _nsdl_interface->create_endpoint(_endpoint_name,
                                     _endpoint_type,
                                     _life_time,
                                     _domain,
                                     (uint8_t)_binding_mode,
                                     _context_address);

    //Here we must use TCP still
    _connection_handler = M2MConnectionHandlerFactory::createConnectionHandler(*this, mode,stack);

    _connection_handler->bind_connection(_listen_port);
     tr_debug("M2MInterfaceImpl::M2MInterfaceImpl() -OUT");
}


M2MInterfaceImpl::~M2MInterfaceImpl()
{
    tr_debug("M2MInterfaceImpl::~M2MInterfaceImpl() - IN");
    delete _nsdl_interface;
    _connection_handler->stop_listening();
    delete _connection_handler;
    tr_debug("M2MInterfaceImpl::~M2MInterfaceImpl() - OUT");
}

void M2MInterfaceImpl::bootstrap(M2MSecurity *security)
{
    tr_debug("M2MInterfaceImpl::bootstrap(M2MSecurity *security) - IN");
    // Transition to a new state based upon
    // the current state of the state machine
    M2MSecurityData* data = new M2MSecurityData();
    data->_object = security;
    BEGIN_TRANSITION_MAP                                    // - Current State -
        TRANSITION_MAP_ENTRY (STATE_BOOTSTRAP)              // state_idle
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_bootstrap
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state__bootstrap_address_resolved
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_bootstrap_resource_created
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_bootstrapped
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_register
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_register_address_resolved
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_register_resource_created
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_registered
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_update_registration
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_unregister
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_unregistered
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_sending_coap_data
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_coap_data_sent
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_coap_data_received
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_processing_coap_data
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_coap_data_processed
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_waiting
    END_TRANSITION_MAP(data)
    if(_event_ignored) {
        _event_ignored = false;
        _observer.error(M2MInterface::NotAllowed);
    }
    tr_debug("M2MInterfaceImpl::bootstrap(M2MSecurity *security) - OUT");
}

void M2MInterfaceImpl::cancel_bootstrap()
{
//TODO: Do we need this ?
}

void M2MInterfaceImpl::register_object(M2MSecurity *security, const M2MObjectList &object_list)
{
    tr_debug("M2MInterfaceImpl::register_object(M2MSecurity *security,const M2MObjectList &object_list) - IN");
    // Transition to a new state based upon
    // the current state of the state machine
    //TODO: manage register object in a list.
    if(!_register_ongoing) {
       _register_ongoing = true;
        _register_server = security;
        M2MRegisterData *data = new M2MRegisterData();
        data->_object = security;
        data->_object_list = object_list;
        BEGIN_TRANSITION_MAP                                    // - Current State -
            TRANSITION_MAP_ENTRY (STATE_REGISTER)               // state_idle
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_bootstrap
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state__bootstrap_address_resolved
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_bootstrap_resource_created
            TRANSITION_MAP_ENTRY (STATE_REGISTER)               // state_bootstrapped
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_register
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_register_address_resolved
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_register_resource_created
            TRANSITION_MAP_ENTRY (STATE_REGISTER)               // state_registered
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_update_registration
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_unregister
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_unregistered
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_sending_coap_data
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_coap_data_sent
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_coap_data_received
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_processing_coap_data
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_coap_data_processed
            TRANSITION_MAP_ENTRY (STATE_REGISTER)               // state_waiting
        END_TRANSITION_MAP(data)
        if(_event_ignored) {
            _event_ignored = false;
            _observer.error(M2MInterface::NotAllowed);
        }
    } else {
       tr_debug("M2MInterfaceImpl::register_object(M2MSecurity *security,const M2MObjectList &object_list) - NOT ALLOWED");
        _observer.error(M2MInterface::NotAllowed);
    }
    tr_debug("M2MInterfaceImpl::register_object(M2MSecurity *security,const M2MObjectList &object_list) - OUT");
}

void M2MInterfaceImpl::update_registration(M2MSecurity *security_object, const uint32_t lifetime)
{
    tr_debug("M2MInterfaceImpl::update_registration(M2MSecurity *security,const uint32_t lifetime) - IN");
    // Transition to a new state based upon
    // the current state of the state machine
    if(lifetime != 0 && (lifetime < MINIMUM_REGISTRATION_TIME)) {
        _observer.error(M2MInterface::InvalidParameters);
    } else if(!_update_register_ongoing){
        _update_register_ongoing = true;
        M2MUpdateRegisterData *data = new M2MUpdateRegisterData();
        data->_object = security_object;
        data->_lifetime = lifetime;
        BEGIN_TRANSITION_MAP                                    // - Current State -
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_idle
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_bootstrap
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state__bootstrap_address_resolved
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_bootstrap_resource_created
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_bootstrapped
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_register
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_register_address_resolved
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_register_resource_created
            TRANSITION_MAP_ENTRY (STATE_UPDATE_REGISTRATION)    // state_registered
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_update_registration
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_unregister
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_unregistered
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_sending_coap_data
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_coap_data_sent
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_coap_data_received
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_processing_coap_data
            TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_coap_data_processed
            TRANSITION_MAP_ENTRY (STATE_UPDATE_REGISTRATION)    // state_waiting
        END_TRANSITION_MAP(data)
        if(_event_ignored) {
            _event_ignored = false;
            _observer.error(M2MInterface::NotAllowed);
        }
    } else {
        tr_debug("M2MInterfaceImpl::update_registration(M2MSecurity *security,const M2MObjectList &object_list) - NOT ALLOWED");
        _observer.error(M2MInterface::NotAllowed);
    }
    tr_debug("M2MInterfaceImpl::update_registration(M2MSecurity *security,const uint32_t lifetime) - OUT");
}

void M2MInterfaceImpl::unregister_object(M2MSecurity* /*security*/)
{
    tr_debug("M2MInterfaceImpl::unregister_object(M2MSecurity *security) - IN");
    // Transition to a new state based upon
    // the current state of the state machine
    BEGIN_TRANSITION_MAP                                // - Current State -
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_idle
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_bootstrap
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state__bootstrap_address_resolved
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_bootstrap_resource_created
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_bootstrapped
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_register
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_register_address_resolved
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_register_resource_created
        TRANSITION_MAP_ENTRY (STATE_UNREGISTER)             // state_registered
        TRANSITION_MAP_ENTRY (STATE_UNREGISTER)             // state_update_registration
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_unregister
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_unregistered
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_sending_coap_data
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_coap_data_sent
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_coap_data_received
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_processing_coap_data
        TRANSITION_MAP_ENTRY (EVENT_IGNORED)                // state_coap_data_processed
        TRANSITION_MAP_ENTRY (STATE_UNREGISTER)             // state_waiting
    END_TRANSITION_MAP(NULL)
    if(_event_ignored) {
        _event_ignored = false;
        _observer.error(M2MInterface::NotAllowed);
    }
    tr_debug("M2MInterfaceImpl::unregister_object(M2MSecurity *security) - OUT");
}

void M2MInterfaceImpl::coap_message_ready(uint8_t *data_ptr,
                                          uint16_t data_len,
                                          sn_nsdl_addr_s *address_ptr)
{
    tr_debug("M2MInterfaceImpl::coap_message_ready(uint8_t *data_ptr,uint16_t data_len,sn_nsdl_addr_s *address_ptr)");
    internal_event(STATE_SENDING_COAP_DATA);
    if(!_connection_handler->send_data(data_ptr,data_len,address_ptr)) {
        internal_event( STATE_IDLE);
        tr_error("M2MInterfaceImpl::coap_message_ready() - M2MInterface::NetworkError");
        _observer.error(M2MInterface::NetworkError);
    }
}

void M2MInterfaceImpl::client_registered(M2MServer *server_object)
{
    tr_debug("M2MInterfaceImpl::client_registered(M2MServer *server_object)");
    internal_event(STATE_REGISTERED);
    //Inform client is registered.
    //TODO: manage register object in a list.
    _observer.object_registered(_register_server,*server_object);
}

void M2MInterfaceImpl::registration_updated(const M2MServer &server_object)
{
    tr_debug("M2MInterfaceImpl::registration_updated(const M2MServer &server_object)");
    internal_event(STATE_REGISTERED);
    _observer.registration_updated(_register_server,server_object);
}


void M2MInterfaceImpl::registration_error(uint8_t error_code)
{
    tr_debug("M2MInterfaceImpl::registration_error(uint8_t error_code) %d", error_code);
    internal_event(STATE_IDLE);
    _observer.error((M2MInterface::Error)error_code);
}

void M2MInterfaceImpl::client_unregistered()
{
    tr_debug("M2MInterfaceImpl::client_unregistered()");
    internal_event(STATE_UNREGISTERED);
    //TODO: manage register object in a list.
    _observer.object_unregistered(_register_server);
}

void M2MInterfaceImpl::bootstrap_done(M2MSecurity *security_object)
{
    tr_debug("M2MInterfaceImpl::bootstrap_done(M2MSecurity *security_object)");
    internal_event(STATE_BOOTSTRAPPED);
    _observer.bootstrap_done(security_object);
}

void M2MInterfaceImpl::bootstrap_error()
{
    tr_debug("M2MInterfaceImpl::bootstrap_error()");
    internal_event(STATE_IDLE);
    _observer.error(M2MInterface::BootstrapFailed);
}

void M2MInterfaceImpl::coap_data_processed()
{
    tr_debug("M2MInterfaceImpl::coap_data_processed()");
    internal_event(STATE_COAP_DATA_PROCESSED);
}

void M2MInterfaceImpl::value_updated(M2MBase *base)
{
    tr_debug("M2MInterfaceImpl::value_updated(M2MBase *base)");
    if(base) {
        M2MBase::BaseType type = base->base_type();
        _observer.value_updated(base, type);
    }
}

void M2MInterfaceImpl::data_available(uint8_t* data,
                                      uint16_t data_size,
                                      const M2MConnectionObserver::SocketAddress &address)
{
    tr_debug("M2MInterfaceImpl::data_available(uint8_t* data,uint16_t data_size,const M2MConnectionObserver::SocketAddress &address)");
    ReceivedData *event = new ReceivedData();
    event->_data = data;
    event->_size = data_size;
    event->_address = &address;
    internal_event(STATE_COAP_DATA_RECEIVED, event);
}

void M2MInterfaceImpl::socket_error(uint8_t /*error_code*/)
{
    tr_debug("M2MInterfaceImpl::socket_error(uint8_t error_code)");
    internal_event(STATE_IDLE);
    M2MInterface::Error error = M2MInterface::NetworkError;
    _observer.error(error);
}

void M2MInterfaceImpl::address_ready(const M2MConnectionObserver::SocketAddress &address,
                                     M2MConnectionObserver::ServerType server_type,
                                     const uint16_t server_port)
{
    tr_debug("M2MInterfaceImpl::address_ready(const M2MConnectionObserver::SocketAddress ,M2MConnectionObserver::ServerType,const uint16_t)");
    ResolvedAddressData *data = new ResolvedAddressData();
    data->_address = &address;
    data->_port = server_port;
    if( M2MConnectionObserver::Bootstrap == server_type) {
        tr_debug("M2MInterfaceImpl::address_ready() Server Type Bootstrap");
        internal_event(STATE_BOOTSTRAP_ADDRESS_RESOLVED, data);
    } else {
        tr_debug("M2MInterfaceImpl::address_ready() Server Type LWM2M");
        internal_event(STATE_REGISTER_ADDRESS_RESOLVED, data);
    }
}

void M2MInterfaceImpl::data_sent()
{
    tr_debug("M2MInterfaceImpl::data_sent()");
    internal_event(STATE_COAP_DATA_SENT);
}

// state machine sits here.
void M2MInterfaceImpl::state_idle(EventData* /*data*/)
{
    // Handle Idle state here
    // Cleanup all resources, if necessary
    _connection_handler->stop_listening();
    _nsdl_interface->stop_timers();
    _register_ongoing = false;
    _update_register_ongoing = false;
    tr_debug("M2MInterfaceImpl::state_idle");
}

void M2MInterfaceImpl::state_bootstrap( EventData *data)
{
    tr_debug("M2MInterfaceImpl::state_bootstrap");
    // Start with bootstrapping preparation
    bool success = false;
    if(data) {
        M2MSecurityData *event = (M2MSecurityData *)data;
        M2MSecurity *security = event->_object;
        if(security) {
            if(M2MSecurity::Bootstrap == security->server_type()) {
                tr_debug("M2MInterfaceImpl::state_bootstrap - server_type : M2MSecurity::Bootstrap");
                String server_address = security->resource_value_string(M2MSecurity::M2MServerUri);
                tr_debug("M2MInterfaceImpl::state_bootstrap - server_address %s", server_address.c_str());
                String ip_address;
                uint16_t port;
                if(server_address.compare(0,COAP.size(),COAP) == 0) {
                    server_address = server_address.substr(COAP.size(),
                                                           server_address.size()-COAP.size());
                    int colonFound = server_address.find_last_of(':');
                    if(colonFound != -1) {
                       ip_address = server_address.substr(0,colonFound);
                       port = atoi(server_address.substr(colonFound+1,
                                                         server_address.size()-ip_address.size()).c_str());

                        tr_debug("M2MInterfaceImpl::state_bootstrap - IP address %s , Port %d", ip_address.c_str(), port);
                       // If bind and resolving server address succeed then proceed else
                       // return error to the application and go to Idle state.
                       if(_connection_handler->resolve_server_address(ip_address,
                                                                      port,
                                                                      M2MConnectionObserver::Bootstrap,
                                                                      security)) {
                           tr_debug("M2MInterfaceImpl::state_bootstrap - resolve_server_address - success");
                           success = true;
                       }
                    }
                }
            }
        }
    }
    if(!success) {
        tr_error("M2MInterfaceImpl::state_bootstrap - M2MInterface::InvalidParameters");
        _observer.error(M2MInterface::InvalidParameters);
        internal_event(STATE_IDLE);
    }
}

void M2MInterfaceImpl::state_bootstrap_address_resolved( EventData *data)
{
    tr_debug("M2MInterfaceImpl::state_bootstrap_address_resolved");
    ResolvedAddressData *event = (ResolvedAddressData *)data;
    sn_nsdl_addr_s address;

    M2MInterface::NetworkStack stack = event->_address->_stack;

    if(M2MInterface::LwIP_IPv4 == stack) {
        tr_debug("M2MInterfaceImpl::state_bootstrap_address_resolved : IPv4 address");
        address.type = SN_NSDL_ADDRESS_TYPE_IPV4;
        address.addr_len = 4;
    } else if((M2MInterface::LwIP_IPv6 == stack) ||
              (M2MInterface::Nanostack_IPv6 == stack)) {
        tr_debug("M2MInterfaceImpl::state_bootstrap_address_resolved : IPv6 address");
        address.type = SN_NSDL_ADDRESS_TYPE_IPV6;
        address.addr_len = 16;
    }
    address.port = event->_port;
    address.addr_ptr = (uint8_t*)event->_address->_address;
    _connection_handler->start_listening_for_data();
    if(_nsdl_interface->create_bootstrap_resource(&address)) {
       tr_debug("M2MInterfaceImpl::state_bootstrap_address_resolved : create_bootstrap_resource - success");
       internal_event(STATE_BOOTSTRAP_RESOURCE_CREATED);
    } else{
        // If resource creation fails then inform error to application
        tr_error("M2MInterfaceImpl::state_bootstrap_address_resolved : M2MInterface::InvalidParameters");
        internal_event(STATE_IDLE);
        _observer.error(M2MInterface::InvalidParameters);
    }
}

void M2MInterfaceImpl::state_bootstrap_resource_created( EventData */*data*/)
{
    tr_debug("M2MInterfaceImpl::state_bootstrap_resource_created");
}

void M2MInterfaceImpl::state_bootstrapped( EventData */*data*/)
{
    tr_debug("M2MInterfaceImpl::state_bootstrapped");
}

void M2MInterfaceImpl::state_register( EventData *data)
{
    tr_debug("M2MInterfaceImpl::state_register");
    // Start with registration preparation
    bool success = false;
    if(data) {
        M2MRegisterData *event = (M2MRegisterData *)data;
        M2MSecurity *security = event->_object;
        if(security) {
            if(M2MSecurity::M2MServer == security->server_type()) {
                tr_debug("M2MInterfaceImpl::state_register - server_type : M2MSecurity::M2MServer");
                if(_nsdl_interface->create_nsdl_list_structure(event->_object_list)) {
                    tr_debug("M2MInterfaceImpl::state_register - create_nsdl_list_structure - success");
                    // If the nsdl resource structure is created successfully
                    String server_address = security->resource_value_string(M2MSecurity::M2MServerUri);
                    tr_debug("M2MInterfaceImpl::state_register - server_address %s", server_address.c_str());
                    String ip_address;
                    uint16_t port;
                    if(server_address.compare(0,COAP.size(),COAP) == 0) {
                        server_address = server_address.substr(COAP.size(),
                                                               server_address.size()-COAP.size());
                        int colonFound = server_address.find_last_of(':'); //10
                        if(colonFound != -1) {
                           ip_address = server_address.substr(0,colonFound);
                           port = atoi(server_address.substr(colonFound+1,
                                                             server_address.size()-ip_address.size()).c_str());

                            tr_debug("M2MInterfaceImpl::state_register - IP address %s , Port %d", ip_address.c_str(), port);
                           // If bind and resolving server address succeed then proceed else
                           // return error to the application and go to Idle state.
                           if(_connection_handler->resolve_server_address(ip_address,
                                                                          port,
                                                                          M2MConnectionObserver::LWM2MServer,
                                                                          security)) {
                              tr_debug("M2MInterfaceImpl::state_register - resolve_server_address - success");
                               success = true;
                           }
                        }
                    }
                }
            }
        }
    }
    if(!success) {
        tr_error("M2MInterfaceImpl::state_register - M2MInterface::InvalidParameters");
        internal_event(STATE_IDLE);
        _observer.error(M2MInterface::InvalidParameters);
    }
}

void M2MInterfaceImpl::state_register_address_resolved( EventData *data)
{
    tr_debug("M2MInterfaceImpl::state_register_address_resolved");
    if(data) {
        ResolvedAddressData *event = (ResolvedAddressData *)data;

        sn_nsdl_addr_type_e address_type = SN_NSDL_ADDRESS_TYPE_IPV6;

        M2MInterface::NetworkStack stack = event->_address->_stack;

        if(M2MInterface::LwIP_IPv4 == stack) {
            tr_debug("M2MInterfaceImpl::state_register_address_resolved : IPv4 address");
            address_type = SN_NSDL_ADDRESS_TYPE_IPV4;
        } else if((M2MInterface::LwIP_IPv6 == stack) ||
                  (M2MInterface::Nanostack_IPv6 == stack)) {
            tr_debug("M2MInterfaceImpl::state_register_address_resolved : IPv6 address");
            address_type = SN_NSDL_ADDRESS_TYPE_IPV6;
        }
        internal_event(STATE_REGISTER_RESOURCE_CREATED);
        _connection_handler->start_listening_for_data();
        if(!_nsdl_interface->send_register_message((uint8_t*)event->_address->_address,event->_port, address_type)) {
            // If resource creation fails then inform error to application
            tr_error("M2MInterfaceImpl::state_register_address_resolved : M2MInterface::InvalidParameters");
            internal_event(STATE_IDLE);
            _observer.error(M2MInterface::InvalidParameters);
        }
    }
}

void M2MInterfaceImpl::state_register_resource_created( EventData */*data*/)
{
    tr_debug("M2MInterfaceImpl::state_register_resource_created");
}

void M2MInterfaceImpl::state_registered( EventData */*data*/)
{
    tr_debug("M2MInterfaceImpl::state_registered");
    _register_ongoing = false;
    _update_register_ongoing = false;
}

void M2MInterfaceImpl::state_update_registration( EventData *data)
{
    tr_debug("M2MInterfaceImpl::state_update_registration");
    // Start with registration preparation
    bool success = false;
    if(data) {
        M2MUpdateRegisterData *event = (M2MUpdateRegisterData *)data;
        success = _nsdl_interface->send_update_registration(event->_lifetime);

    }
    if(!success) {
        tr_error("M2MInterfaceImpl::state_register_address_resolved : M2MInterface::InvalidParameters");
        internal_event(STATE_IDLE);
        _observer.error(M2MInterface::InvalidParameters);
    }
}

void M2MInterfaceImpl::state_unregister( EventData */*data*/)
{
    tr_debug("M2MInterfaceImpl::state_unregister");
    internal_event(STATE_SENDING_COAP_DATA);
    if(!_nsdl_interface->send_unregister_message()) {
        tr_error("M2MInterfaceImpl::state_unregister : M2MInterface::NotRegistered");
        internal_event(STATE_IDLE);
        _observer.error(M2MInterface::NotRegistered);
    }
}

void M2MInterfaceImpl::state_unregistered( EventData */*data*/)
{
    tr_debug("M2MInterfaceImpl::state_unregistered");
    internal_event(STATE_IDLE);
}

void M2MInterfaceImpl::state_sending_coap_data( EventData */*data*/)
{
    tr_debug("M2MInterfaceImpl::state_sending_coap_data");
    internal_event(STATE_WAITING);
}

void M2MInterfaceImpl::state_coap_data_sent( EventData */*data*/)
{
    tr_debug("M2MInterfaceImpl::state_coap_data_sent");
    internal_event(STATE_WAITING);
}

void M2MInterfaceImpl::state_coap_data_received( EventData *data)
{
    tr_debug("M2MInterfaceImpl::state_coap_data_received");
    if(data) {
        ReceivedData *event = (ReceivedData*)data;
        sn_nsdl_addr_s address;

        M2MInterface::NetworkStack stack = event->_address->_stack;

        if(M2MInterface::LwIP_IPv4 == stack) {
            tr_debug("M2MInterfaceImpl::state_coap_data_received : IPv4 address");
            address.type = SN_NSDL_ADDRESS_TYPE_IPV4;
            address.addr_len = 4;
        } else if((M2MInterface::LwIP_IPv6 == stack) ||
                  (M2MInterface::Nanostack_IPv6 == stack)) {
            tr_debug("M2MInterfaceImpl::state_coap_data_received : IPv6 address");
            address.type = SN_NSDL_ADDRESS_TYPE_IPV6;
            address.addr_len = 16;
        }
        address.port = event->_address->_port;
        address.addr_ptr = (uint8_t*)event->_address->_address;

        // Process received data
        internal_event(STATE_PROCESSING_COAP_DATA);
        if(!_nsdl_interface->process_received_data(event->_data,
                                                  event->_size,
                                                  &address)) {
           tr_error("M2MInterfaceImpl::state_coap_data_received : M2MInterface::ResponseParseFailed");
            _observer.error(M2MInterface::ResponseParseFailed);
        }
    }
}

void M2MInterfaceImpl::state_processing_coap_data( EventData */*data*/)
{
    tr_debug("M2MInterfaceImpl::state_processing_coap_data");
    internal_event(STATE_WAITING);
}

void M2MInterfaceImpl::state_coap_data_processed( EventData */*data*/)
{
    tr_debug("M2MInterfaceImpl::state_coap_data_processed");
    internal_event(STATE_WAITING);
}

void M2MInterfaceImpl::state_waiting( EventData */*data*/)
{
    tr_debug("M2MInterfaceImpl::state_waiting");
}

// generates an external event. called once per external event
// to start the state machine executing
void M2MInterfaceImpl::external_event(uint8_t new_state,
                                     EventData* p_data)
{
    tr_debug("M2MInterfaceImpl::external_event : new state %d", new_state);
    // if we are supposed to ignore this event
    if (new_state == EVENT_IGNORED) {
        tr_debug("M2MInterfaceImpl::external_event : new state is EVENT_IGNORED");
        // just delete the event data, if any
        if (p_data) {
            delete p_data;
            p_data = NULL;
        }
        _event_ignored = true;
    }
    else {
        tr_debug("M2MInterfaceImpl::external_event : handle new state");
        // generate the event and execute the state engine
        internal_event(new_state, p_data);
    }
}

// generates an internal event. called from within a state
// function to transition to a new state
void M2MInterfaceImpl::internal_event(uint8_t new_state,
                                      EventData* p_data)
{
    tr_debug("M2MInterfaceImpl::internal_event : new state %d", new_state);
    _event_data = p_data;
    _event_generated = true;
    _current_state = new_state;
    state_engine();
}

// the state engine executes the state machine states
void M2MInterfaceImpl::state_engine (void )
{
    tr_debug("M2MInterfaceImpl::state_engine");
    EventData* p_data_temp = NULL;

    // while events are being generated keep executing states
    while (_event_generated) {
        p_data_temp = _event_data;  // copy of event data pointer
        _event_data = NULL;       // event data used up, reset ptr
        _event_generated = false;  // event used up, reset flag

        assert(_current_state < _max_states);

        state_function( _current_state, p_data_temp );

        // if event data was used, then delete it
        if (p_data_temp) {
            delete p_data_temp;
            p_data_temp = NULL;
        }
    }
}

void M2MInterfaceImpl::state_function( uint8_t current_state, EventData* data )
{
    switch( current_state ) {
    case STATE_IDLE:
            M2MInterfaceImpl::state_idle(data);
            break;
        case STATE_BOOTSTRAP:
            M2MInterfaceImpl::state_bootstrap(data);
            break;
        case STATE_BOOTSTRAP_ADDRESS_RESOLVED:
            M2MInterfaceImpl::state_bootstrap_address_resolved(data);
            break;
        case STATE_BOOTSTRAP_RESOURCE_CREATED:
            M2MInterfaceImpl::state_bootstrap_resource_created(data);
            break;
        case STATE_BOOTSTRAPPED:
            M2MInterfaceImpl::state_bootstrapped(data);
            break;
        case STATE_REGISTER:
            M2MInterfaceImpl::state_register(data);
            break;
        case STATE_REGISTER_ADDRESS_RESOLVED:
            M2MInterfaceImpl::state_register_address_resolved(data);
            break;
        case STATE_REGISTER_RESOURCE_CREATED:
            M2MInterfaceImpl::state_register_resource_created(data);
            break;
        case STATE_REGISTERED:
            M2MInterfaceImpl::state_registered(data);
            break;
        case STATE_UPDATE_REGISTRATION:
            M2MInterfaceImpl::state_update_registration(data);
            break;
        case STATE_UNREGISTER:
            M2MInterfaceImpl::state_unregister(data);
            break;
        case STATE_UNREGISTERED:
            M2MInterfaceImpl::state_unregistered(data);
            break;
        case STATE_SENDING_COAP_DATA:
            M2MInterfaceImpl::state_sending_coap_data(data);
            break;
        case STATE_COAP_DATA_SENT:
            M2MInterfaceImpl::state_coap_data_sent(data);
            break;
        case STATE_COAP_DATA_RECEIVED:
            M2MInterfaceImpl::state_coap_data_received(data);
            break;
        case STATE_PROCESSING_COAP_DATA:
            M2MInterfaceImpl::state_processing_coap_data(data);
            break;
        case STATE_COAP_DATA_PROCESSED:
            M2MInterfaceImpl::state_coap_data_processed(data);
            break;
        case STATE_WAITING:
            M2MInterfaceImpl::state_waiting(data);
            break;
    }
}
