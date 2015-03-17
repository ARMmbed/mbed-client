/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_NSDL_OBSERVER_H
#define M2M_NSDL_OBSERVER_H

#include "nsdllinker.h"

//FORWARD DECLARATION
class M2MSecurity;
class M2MServer;

/**
 * @brief Observer class for informing NSDL callback to the state machine
 */

class M2MNsdlObserver
{

public :

    /**
    * @brief Informs that coap message is ready.
    */
    virtual void coap_message_ready(uint8_t *data_ptr,
                                    uint16_t data_len,
                                    sn_nsdl_addr_s *address_ptr) = 0;

    /**
    * @brief Informs that client is registered successfully.
    * @param server_object, Server object associated with
    * registered server.
    */
    virtual void client_registered(M2MServer *server_object) = 0;

    /**
    * @brief Informs that some error occured during
    * registration.
    * @param error_code, Error code for registration error
    */
    virtual void registration_error(uint8_t error_code) = 0;

    /**
    * @brief Informs that client is unregistered successfully.
    */
    virtual void client_unregistered() = 0;

    /**
    * @brief Informs that client bootstrapping is done.
    * @param security_object, M2MSecurity Object which contains information about
    * LWM2M server fetched from bootstrap server.
    */
    virtual void bootstrap_done(M2MSecurity *security_object) = 0;

    /**
    * @brief Informs that some error occured during
    * bootstrapping.
    */
    virtual void bootstrap_error() = 0;

    /**
    * @brief Informs that received data has been processed.
    */
    virtual void coap_data_processed() = 0;

};
#endif // M2M_NSDL_OBSERVER_H
