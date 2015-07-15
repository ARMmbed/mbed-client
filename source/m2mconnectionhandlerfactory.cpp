
#include "lwm2m-client/m2mconfig.h"
#include "lwm2m-client/m2mconnectionhandlerfactory.h"
#include "lwm2m-client/m2mconnectionsecurity.h"
#include "lwm2m-client/m2mconnectionhandler.h"


#include "lwm2m-client/m2mconnectionsecurity.h"
#include "lwm2m-client/m2mconnectionhandler.h"

M2MConnectionHandler* M2MConnectionHandlerFactory::createConnectionHandler(M2MConnectionObserver &observer,
                                                                           M2MInterface::NetworkStack stack){
    M2MConnectionHandler* ret = NULL;
    M2MConnectionSecurity* sec = NULL;

    sec = new M2MConnectionSecurity();

    ret = new M2MConnectionHandler(observer, sec, stack);

    return ret;
}
