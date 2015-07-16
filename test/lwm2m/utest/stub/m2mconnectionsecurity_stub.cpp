
#include "lwm2m-client/m2mconnectionhandler.h"
#include "lwm2m-client/m2mconnectionsecurity.h"
#include "lwm2m-client/m2mtimer.h"
#include "lwm2m-client/m2msecurity.h"
#include <string.h>
#include "m2mconnectionsecurity_stub.h"

bool m2mconnectionsecurityimpl_stub::use_inc_int;
int m2mconnectionsecurityimpl_stub::inc_int_value;
int m2mconnectionsecurityimpl_stub::int_value;

void m2mconnectionsecurityimpl_stub::clear()
{
    int_value = -1;
    use_inc_int = false;
    inc_int_value = 0;
}

M2MConnectionSecurity::M2MConnectionSecurity()
{

}

M2MConnectionSecurity::~M2MConnectionSecurity(){
}

void M2MConnectionSecurity::reset(){
}

int M2MConnectionSecurity::init(const M2MSecurity *security){
    if(m2mconnectionsecurityimpl_stub::use_inc_int){
        return m2mconnectionsecurityimpl_stub::inc_int_value++;
    }
    return m2mconnectionsecurityimpl_stub::int_value;
}

int M2MConnectionSecurity::connect(M2MConnectionHandler* connHandler){
    if(m2mconnectionsecurityimpl_stub::use_inc_int){
        return m2mconnectionsecurityimpl_stub::inc_int_value++;
    }
    return m2mconnectionsecurityimpl_stub::int_value;
}

int M2MConnectionSecurity::start_connecting_non_blocking(M2MConnectionHandler* connHandler)
{
    if(m2mconnectionsecurityimpl_stub::use_inc_int){
        return m2mconnectionsecurityimpl_stub::inc_int_value++;
    }
    return m2mconnectionsecurityimpl_stub::int_value;
}

int M2MConnectionSecurity::continue_connecting()
{
    if(m2mconnectionsecurityimpl_stub::use_inc_int){
        return m2mconnectionsecurityimpl_stub::inc_int_value++;
    }
    return m2mconnectionsecurityimpl_stub::int_value;
}

int M2MConnectionSecurity::send_message(unsigned char *message, int len){
    if(m2mconnectionsecurityimpl_stub::use_inc_int){
        return m2mconnectionsecurityimpl_stub::inc_int_value++;
    }
    return m2mconnectionsecurityimpl_stub::int_value;
}

int M2MConnectionSecurity::read(unsigned char* buffer, uint16_t len){
    if(m2mconnectionsecurityimpl_stub::use_inc_int){
        return m2mconnectionsecurityimpl_stub::inc_int_value++;
    }
    return m2mconnectionsecurityimpl_stub::int_value;
}
