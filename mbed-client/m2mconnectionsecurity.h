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
#ifndef __M2M_CONNECTION_SECURITY_H__
#define __M2M_CONNECTION_SECURITY_H__

class M2MConnectionHandler;
class M2MSecurity;
class M2MConnectionSecurityPimpl;

/**
 * @brief M2MConnectionSecurity.
 * This class provides a method to create a secure socket connection
 * to handle connectivity for the mbed Client. It will handle sending, receiving
 * and establishing a secure connection for the mbed Client on top of the
 * normal socket connection.
 */

class M2MConnectionSecurity {
public:
    typedef enum {
        NO_SECURITY = 0,
        TLS,
        DTLS
    } SecurityMode;

private:
    // Prevents the use of assignment operator by accident.
    M2MConnectionSecurity& operator=( const M2MConnectionSecurity& /*other*/ );
    // Prevents the use of copy constructor by accident.
    M2MConnectionSecurity( const M2MConnectionSecurity& /*other*/ );

public:
    /**
     * @brief Default Constructor.
     */
    M2MConnectionSecurity(SecurityMode mode);

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
     * @brief Starts the connection in non-blocking mode.
     * @param connHandler, ConnectionHandler object that maintains the socket.
     * @return Returns the state of the connection. Successful or not.
     */
    int start_connecting_non_blocking(M2MConnectionHandler* connHandler);

    /**
     * @brief Continues connectivity logic for secure connection.
     * @return Returns error code if any while continuing connection sequence.
     */
    int continue_connecting();

    /**
     * @brief Connects the client to the server.
     * @param connHandler, ConnectionHandler object that maintains the socket.
     * @return Returns the state of the connection. Successful or not.
     */
    int connect(M2MConnectionHandler* connHandler);

    /**
     * @brief Sends data to the server.
     * @param message, Data to be sent.
     * @param len, Length of the data.
     * @return Indicates whether the data is sent successfully or not.
     */
    int send_message(unsigned char *message, int len);

    /**
     * @brief Reads the data received from the server.
     * @param message, Data to be read.
     * @param len, Length of the data.
     * @return Indicates whether the data is read successfully or not.
     */
    int read(unsigned char* buffer, uint16_t len);

private:

    M2MConnectionSecurityPimpl* _private_impl;

    friend class Test_M2MConnectionSecurity;
    //friend class Test_M2MConnectionSecurityImpl;
};

#endif //__M2M_CONNECTION_SECURITY_H__
