
#include "lwm2m-client/m2mconnectionhandler.h"
#include "lwm2m-client/m2mconnectionsecurity.h"
#include "lwm2m-client-mbedtls/m2mconnectionsecuritypimpl.h"
#include "lwm2m-client/m2mtimer.h"
#include "lwm2m-client/m2msecurity.h"
#include <string.h>
#include "m2mconnectionsecuritypimpl_stub.h"

int m2mconnectionsecuritypimpl_stub::int_value;

void m2mconnectionsecuritypimpl_stub::clear()
{
    int_value = -1;
}

M2MConnectionSecurityPimpl::M2MConnectionSecurityPimpl()
{

}

M2MConnectionSecurityPimpl::~M2MConnectionSecurityPimpl(){
}

void M2MConnectionSecurityPimpl::reset(){
}

int M2MConnectionSecurityPimpl::init(const M2MSecurity *security){
    return m2mconnectionsecuritypimpl_stub::int_value;
}

int M2MConnectionSecurityPimpl::connect(M2MConnectionHandler* connHandler){
    return m2mconnectionsecuritypimpl_stub::int_value;
}

int M2MConnectionSecurityPimpl::start_connecting_non_blocking(M2MConnectionHandler* connHandler)
{
    return m2mconnectionsecuritypimpl_stub::int_value;
}

int M2MConnectionSecurityPimpl::continue_connecting()
{
    return m2mconnectionsecuritypimpl_stub::int_value;
}

int M2MConnectionSecurityPimpl::send_message(unsigned char *message, int len){
    return m2mconnectionsecuritypimpl_stub::int_value;
}

int M2MConnectionSecurityPimpl::read(unsigned char* buffer, uint16_t len){
    return m2mconnectionsecuritypimpl_stub::int_value;
}

void M2MConnectionSecurityPimpl::timer_expired(M2MTimerObserver::Type type){
}
