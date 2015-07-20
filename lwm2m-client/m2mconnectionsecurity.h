#ifndef __M2M_CONNECTION_SECURITY_H__
#define __M2M_CONNECTION_SECURITY_H__

class M2MConnectionHandler;
class M2MSecurity;
class M2MConnectionSecurityPimpl;

/**
 * @brief M2MConnectionSecurity.
 * This class provides method to create secure socket connection
 * to handle connectivity for LWM2M Client. It will handle send, receive
 * and establishing secure conenction for LWM2M Client on top of
 * normal socket connection.
 */

class M2MConnectionSecurity {

private:
    // Prevents the use of assignment operator by accident.
    M2MConnectionSecurity& operator=( const M2MConnectionSecurity& /*other*/ );
    // Prevents the use of copy constructor by accident
    M2MConnectionSecurity( const M2MConnectionSecurity& /*other*/ );

public:
    /**
     * @brief Default Constructor.
     */
    M2MConnectionSecurity();

    /**
     * @brief Default Destructor.
     */
    ~M2MConnectionSecurity();

    /**
     * @brief Resets the socket connection states.
     */
    void reset();

    /**
     * @brief Initiatlizes the socket connection states.
     */
    int init(const M2MSecurity *security);

    /**
     * @brief Starts the connection in non blocking mode.
     * @param connHandler, ConnectionHandler object which maintains the socket.
     * @return Returns the state if the connection is successful or not.
     */
    int start_connecting_non_blocking(M2MConnectionHandler* connHandler);

    /**
     * @brief Continues connectivity logic for secure connection.
     * @return Returns error code if any while continuing connection sequence.
     */
    int continue_connecting();

    /**
     * @brief Connects the client to server.
     * @param connHandler, ConnectionHandler object which maintains the socket.
     * @return Returns the state if the connection is successful or not.
     */
    int connect(M2MConnectionHandler* connHandler);

    /**
     * @brief Sends data to the server.
     * @param message, Data to be sent.
     * @param len, Length of the data.
     * @return Returns if the data is sent successfully or not.
     */
    int send_message(unsigned char *message, int len);

    /**
     * @brief Reads the data received from server.
     * @param message, Data to be read.
     * @param len, Length of the data.
     * @return Returns if the data is read successfully or not.
     */
    int read(unsigned char* buffer, uint16_t len);

private:

    M2MConnectionSecurityPimpl* _private_impl;

    friend class Test_M2MConnectionSecurity;
    friend class Test_M2MConnectionSecurityImpl;
};

#endif //__M2M_CONNECTION_SECURITY_H__
