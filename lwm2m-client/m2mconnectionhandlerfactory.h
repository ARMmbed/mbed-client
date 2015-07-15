#ifndef M2MCONNECTIONHANDLERFACTORY_H
#define M2MCONNECTIONHANDLERFACTORY_H

#include "lwm2m-client/m2mconnectionobserver.h"

class M2MConnectionHandler;

class M2MConnectionHandlerFactory {
public:
    /**
     * @brief createConnectionHandler will create and return a connection
     * handler based on current platform setup.
     * It will also setup the security mechanism based on build time macros.
     * By default security is NULL (Not recommended!)
     * @return Platform specific connection handler
     */
    static M2MConnectionHandler* createConnectionHandler(M2MConnectionObserver &observer,
                                                             M2MInterface::NetworkStack stack);
};

#endif // M2MCONNECTIONHANDLERFACTORY_H
