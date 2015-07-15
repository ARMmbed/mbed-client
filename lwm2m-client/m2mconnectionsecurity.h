
#ifndef __M2M_CONNECTION_SECURITY_IMPL_H__
#define __M2M_CONNECTION_SECURITY_IMPL_H__

class M2MConnectionHandler;
class M2MSecurity;
class M2MConnectionSecurityPimpl;

class M2MConnectionSecurity {
private:
    // Prevents the use of assignment operator by accident.
    M2MConnectionSecurity& operator=( const M2MConnectionSecurity& /*other*/ );
    // Prevents the use of copy constructor by accident
    M2MConnectionSecurity( const M2MConnectionSecurity& /*other*/ );

public:
    M2MConnectionSecurity();

    ~M2MConnectionSecurity();

    void reset();

    int init(const M2MSecurity *security);

    int start_connecting_non_blocking(M2MConnectionHandler* connHandler);

    int continue_connecting();

    int connect(M2MConnectionHandler* connHandler);

    int send_message(unsigned char *message, int len);

    int read(unsigned char* buffer, uint16_t len);

private:

    M2MConnectionSecurityPimpl* _private_impl;

    friend class Test_M2MConnectionSecurity;
    friend class Test_M2MConnectionSecurityImpl;
};

#endif //__M2M_CONNECTION_SECURITY_IMPL_H__
