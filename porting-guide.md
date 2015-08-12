# Introduction to mbed Client

mbed Client is structured as a set of modules. Each module declares which other
modules it depends on. When you build a module, our build system `yotta`
looks at these dependencies and installs the necessary modules before
completing the build.

This is also the process to build applications for the mbed Client
(including the example application in the release). The application declares
dependencies on the mbed Client and when it is built,
`yotta` ensures that the modules (and anything that they depend on, recursively)
are present before building. 

In this release, all the necessary modules for the examples (the ones that the
examples depend on) have been pre-installed in the `yotta_modules` directory of
each example program, so you do not need to download them separately. In
general, `yotta` downloads and installs the necessary modules over the internet from the public yotta Registry 
(which saves published versions of modules) or from a specified source control URL.

# Building and testing mbed Client

Before embarking on your own port, you should build the core mbed Client for an existing compilation target to get an understanding of how the mbed Client builds. You can find a general guide for building the mbed Client and its example application in the mbed Client release notes.

# Components of mbed Client

## Component software modules

The mbed Client consists of one main component. More major components will be added in the further development phases:

 * `mbed-client` is the core mbed Client, providing C++ APIs for the mbed Client.

This module depends on further internal modules:

```
mbed-client x.x.x
|
|_mbed-client-c x.x.x
|   |_mbed-client-libservice x.x.x
|
|_mbed-client-linux x.x.x
```

To list the dependency trees, give [`yotta list --all`
command](http://docs.yottabuild.org/reference/commands.html).

**Note**: In this case, we have listed the dependencies for the `x86-linux-native`
compilation target. Different modules are needed for different compilation target.

If you list the modules included in the Mbed Client linux example, you can see that
it depends directly only on the `mbed-client` and `mbed-client-linux` modules. These modules depend internally on various other modules.

```
mbed-client-linux-example x.x.x
|
|_mbed-client x.x.x
   |
   |_mbed-client-c x.x.x
   |   |_mbed-client-libservice x.x.x
   |
   |_mbed-client-linux 0.0.2
```

## Compilation targets

This release supports only one compilation target:

 * `x86-linux-native`: to compile for linux OS, using the gcc toolchain.

To select the compilation target used for compiling a library or example
program, give the command `yotta target x86-linux-native` before building. More
documentation on target descriptions can be found on the [yotta
documentation site](http://docs.yottabuild.org/tutorial/targets.html).

# Porting mbed Client to different platform

To port mbed Client to a new platform, perform the following steps:

 1. Contact [support@mbed.org](mailto:support@mbed.org), to request for new development repositories for your target platform.
 2. Create a yotta compilation target for your board.
 3. Implement the `mbed-client-xxx` module for your target platform where `xxx` is the name for your platform (for example, `linux`).
 4. Make sure that your port is updated into `module.json` of the `mbed-client` module.
 5. Verify that your implementation is correct.

The `yotta` build system is designed for easy reuse of generic modules.
If you intend to support multiple platforms that share common features, it is recommended to store the common 
functionality into a separate module and use it for each platform.


# Step 1: Creating development repositories

We provide private git repositories to our partners porting mbed Client. Only the members of the mbed Client team and relevant partner contacts and engineers have access to these repositories.

When you contact `support@mbed.org`, a repository will be created for your
module. You also need to provide the target description of your board:

- **`mbed-client-<platform-name>`** is the module that provides the `mbed-client-xxx`
   implementation for your platform. You may choose to split it into further modules in the future, to enable sharing
   of code, but we recommend that you implement the port for your first board in this module itself. 

- **`target-<targetname>`** contains the yotta target description of the
   target you are porting to. This is usually your platform name.

# Step 2: Creating a yotta compilation target

To compile for a target board, you need a [target description](http://docs.yottabuild.org/tutorial/targets.html) that describes how to compile for the target. 

The `mbed-client` module uses the platform name that each target defines, to choose which `mbed-client-<platform-name>` module to depend on to provide the platform-specific implementation.

The target description contains:
 * `target.json` - the target description file
 * a [CMake toolchain
   file](http://www.cmake.org/cmake/help/v3.0/manual/cmake-toolchains.7.html)
   (and any dependencies it needs) that describes how to run the compiler
 * a linker script file

To select the target description, run `yotta target <targetname>`. yotta needs this to set up the build.

The documentation for target descriptions is available on the [yotta
documentation site](http://docs.yottabuild.org/tutorial/targets.html).

An example on compiling for linux target can be found in the `yotta_targets` directory of the example application.

The directory structure of a typical target description is:

```
|readme.md
|target.json
|_CMake
|   |_toolchain.cmake
|   |
|   |_Compiler
|   |   |_GNU-ASM.cmake
|   |   |_GNU-C.cmake
|   |   |_GNU-CXX.cmake
|   |
|   |_Platform
|       |_xxx-GNU-C.cmake
|       |_xxx-GNU-CXX.cmake
|       |_xxx.cmake
|
|_ld
    |_newtarget.ld
```

Where the `target.json` file specifies the path to the CMake toolchain (`toolchain.cmake`) that uses the standard CMake mechanisms for including compiler, platform and language specific configuration.

**Note**: Improvements are planned to the yotta target description to allow more reuse of the descriptions between targets.

To make your target available locally (without publishing it), you can give `yotta link-target` command to link it into the global install targets directory:

```
# in the directory of your target:
yotta link-target
```

You can then give `yotta link-target <targetname>` command to make the globally linked target available when compiling another module. After that, give `yotta target <targetname>` command to select your target for the compilation.

# Step 3: Implementing mbed-client-xxx

First, clone your `mbed-client-<your-platform-name>` module and `mbed-client` modules from GitHub.

The `mbed-client-<your-platform-name>` module needs to provide a socket and timer implementation for
your target platform. The mbed-client-xxx module should include files `m2mconnectionhandler.h`and `m2mtimer.h` from `mbed-client` and implement a corresponding `.cpp` file that points to the platform-specific private implementations of the timer and the socket.

**Note**: Private implementation classes MUST be named as `M2MConnectionHandlerPimpl` and `M2MTimerPimpl`, because of forward declarations.

An example of mbed-client-platform core:
```
|_module.json
|
|_mbed-client-platform
|    |_m2mconnectionhandlerpimpl.h
|    |_m2mtimerpimpl.h
|
|_source
    |_m2mconnectionhandler.cpp
    |_m2mconnectionhandlerpimpl.cpp
    |_m2mtimer.cpp
    |_m2mtimerpimpl.cpp
```

To make your module available to other modules that you want to build, you
need to give [`yotta link`](http://docs.yottabuild.org/reference/commands.html#yotta-link) command
to link it to the module where you want to test it out.

To use your local your in-development mbed-client implementation, give the command `yotta link mbed-client-xxx` in the main `mbed-client` module.

```
# in mbed-client, link your module:
yotta link
```

You can also just commit and push your untested code to GitHub, but it is always a good idea to test before committing.

Your `mbed-client-xxx` module must provide a platform-specific implementation for the mbed-client. The APIs that needs porting are defined in the `mbed-client-linux` module. The header files contain documentation alongside the declaration of each function, where the function is described along with its parameters and return value.

There are two header files that require porting for your platform:

1. `m2mconnectionhandler.h`
2. `m2mtimer.h` 

To see how this is done in Linux, check the `mbed-client-linux` module available in this package.

## Implementing M2MConnectionHandler class for your platform

```
/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_CONNECTION_HANDLER_H__
#define M2M_CONNECTION_HANDLER_H__

#include "mbed-client/m2mconfig.h"
#include "mbed-client/m2minterface.h"
#include "mbed-client/m2mconnectionobserver.h"
#include "nsdl-c/sn_nsdl.h"

/**
 * @brief M2MConnectionHandler.
 * This class handles the socket connection for Mbed Client
 */

class M2MConnectionHandler {
public:

    /**
     * @enum ConnectionError
     * This enum defines error which can come from
     * socket read and write operation.
     */
    typedef enum {
        CONNECTION_ERROR_WANTS_READ = -1000,
        CONNECTION_ERROR_WANTS_WRITE = -1001
    }ConnectionError;

public:

    /**
    * @brief Constructor
    */
    M2MConnectionHandler(M2MConnectionObserver &observer,
                         M2MConnectionSecurity* sec,
                         M2MInterface::NetworkStack stack);

    /**
    * @brief Destructor
    */
    ~M2MConnectionHandler();

    /**
    * @brief This binds the socket connection.
    * @param listen_port Port to listen for incoming connection.
    * @return true if successful else false.
    */
    bool bind_connection(const uint16_t listen_port);

    /**
    * @brief This resolves the server address. Output is
    * returned through callback
    * @param String server address.
    * @param uint16_t Server port.
    * @param ServerType, Server Type to be resolved.
    * @param security, M2MSecurity object which determines what
    * kind of secure connection will be used by socket.
    * @return true if address is valid else false.
    */
    bool resolve_server_address(const String& server_address,
                                const uint16_t server_port,
                                M2MConnectionObserver::ServerType server_type,
                                const M2MSecurity* security);

    /**
    * @brief Sends data, to the connected sent to server.
    * @param data_ptr, Data to be sent.
    * @param data_len, Length of data to be sent.
    * @param address_ptr, Address structure where data has to be sent.
    * @return True if data sent is successful else false.
    */
    bool send_data(uint8_t *data_ptr,
                           uint16_t data_len,
                           sn_nsdl_addr_s *address_ptr);

    /**
    * @brief Listens for incoming data from remote server
    * @return true if successful else false.
    */
    bool start_listening_for_data();

    /**
    * @brief Stops listening for incoming data.
    */
    void stop_listening();

    /**
     * @brief sendToSocket Sends directly to socket. This is used by
     * security classes to send after data has been encrypted.
     * @param buf Buffer to send.
     * @param len Length of a buffer.
     * @return Number of bytes sent or -1 if failed.
     */
    int sendToSocket(const unsigned char *buf, size_t len);

    /**
     * @brief receiveFromSocket Receives directly from a socket. This
     * is used by security classes to receive raw data to be decrypted.
     * @param buf Buffer to send.
     * @param len Length of a buffer.
     * @return Number of bytes read or -1 if failed.
     */
    int receiveFromSocket(unsigned char *buf, size_t len);


    /**
    * @brief Closes the open connection.
    */
    void close_connection();

private:

    M2MConnectionObserver                       &_observer;
    M2MConnectionHandlerPimpl                   *_private_impl;

friend class Test_M2MConnectionHandler;
friend class Test_M2MConnectionHandler_mbed;
friend class Test_M2MConnectionHandler_linux;
friend class M2MConnection_TestObserver;
};

#endif //M2M_CONNECTION_HANDLER_H__

```

Please note that some of these functions are asynchronous in nature and some are expecting callback from the network. For example, receiving data from a socket needs to be communicated back to the `mbed-client` so that the library can act on the data received. The callback comes through the Observer class defined in `M2MConnectionObserver`.

The file `m2mconnectionobserver.h` is present in the `mbed-client`. To see how the callback needs to be called, check the implementation in `m2mconnectionhandlerpimpl.cpp` present in the `mbed-client-linux`. 

```
/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_CONNECTION_OBSERVER_H__
#define M2M_CONNECTION_OBSERVER_H__

#include "mbed-client/m2minterface.h"

/**
 * @brief Observer class for informing socket activity to the state machine
 */

class M2MConnectionObserver
{

public :

    typedef enum {
        Bootstrap,
        MbedServer
    }ServerType;

    /**
     * @brief The M2MSocketAddress struct
     * Unified container for holding socket address data
     * across different platforms.
     */
    struct SocketAddress{
        M2MInterface::NetworkStack  _stack;
        void                        *_address;    
        uint8_t                     _length;
        uint16_t                    _port;
    };

    /**
    * @brief Indicates data is available from socket.
    * @param data, data read from socket
    * @param data_size, length of data read from socket.
    * @param address, Server Address from where data is coming.
    */
    virtual void data_available(uint8_t* data,
                                uint16_t data_size,
                                const M2MConnectionObserver::SocketAddress &address) = 0;

    /**
    * @brief Indicates some error occured in socket.
    * @param error_code, Error code from socket.
    * It cannot be used any further.
    */
    virtual void socket_error(uint8_t error_code) = 0;

    /**
    * @brief Indicates server address resolving is ready.
    * @param address, Resolved socket address.
    * @param server_type, Type of server.
    * @param server_port, Port of the resolved server address.
    */
    virtual void address_ready(const M2MConnectionObserver::SocketAddress &address,
                               M2MConnectionObserver::ServerType server_type,
                               const uint16_t server_port) = 0;

    /**
    * @brief Indicates data has been sent successfully.
    */
    virtual void data_sent() = 0;

};

#endif // M2M_CONNECTION_OBSERVER_H__

```

## Implementing M2MTimer class for your platform

```
/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_TIMER_H
#define M2M_TIMER_H

#include <stdint.h>

class M2MTimerObserver;
/**
 * @brief M2MTimerImpl
 * Private implementation class for timer, this can be
 * modified based on platform on which mbed client needs
 * to be used.
 */
class M2MTimerImpl
{
private:

    // Prevents the use of assignment operator
    M2MTimer& operator=(const M2MTimer& other);

    // Prevents the use of copy constructor
    M2MTimer(const M2MTimer& other);
    
public:

    /**
    * Constructor.
    */
    M2MTimer(M2MTimerObserver& _observer);

    /**
    * Destructor.
    */
    virtual ~M2MTimer();

    /**
     * Starts timer
     * @param interval Timer's interval in milliseconds
    * @param single_shot defines if timer is ticked
    * once or is it restarted everytime timer is expired.
    */
    void start_timer(uint64_t interval, bool single_shot = true);

    /**
     * @brief Starts timer in DTLS manner.
     * @param intermediate_interval Intermediate interval to use, must be smaller than tiotal (usually 1/4 of total).
     * @param total_interval Total interval to use; This is the timeout value of a DTLS packet.
     * @param type Type of the timer
     */
    void start_dtls_timer(uint64_t intermediate_interval, uint64_t total_interval, 
                          M2MTimerObserver::Type type = M2MTimerObserver::Dtls);

    /**
    * Stops timer.
    * This cancels the ongoing timer.
    */
    void stop_timer();
    
    /**
     * @brief Checks if the intermediate interval has passed.
     * @return true if interval has passed, false otherwise.
     */
    bool is_intermediate_interval_passed();

    /**
     * @brief Checks if the total interval has passed.
     * @return true if interval has passed, false otherwise.
     */
    bool is_total_interval_passed();

};

#endif // M2M_TIMER_H
```

The timer API functions are asynchronous in nature and whenever a timer event is available, it is notified to the `mbed-client` so that the library can act on the _timer expired_ signal. The callback is received through an Observer class defined in `M2MTimerObserver` .

The file `m2mtimerobserver.h` is present in `mbed-client`. To see how the callback needs to be called, check the implementation in `m2mtimerimpl.cpp` present in the`mbed-client-linux`. 

```
/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_TIMER_OBSERVER_H
#define M2M_TIMER_OBSERVER_H

/**
 *  Observer class for informing timer expiry to the parent class
 */
class M2MTimerObserver
{
public:
    typedef enum {
        Notdefined,
        Registration,
        NsdlExecution,
        PMinTimer,
        PMaxTimer,
        Dtls
    }Type;

    /**
    * Indicates that the time has expired.
    */
    virtual void timer_expired(M2MTimerObserver::Type type =
                               M2MTimerObserver::Notdefined) = 0;
};

#endif // M2M_TIMER_OBSERVER_H
```

# Step 4: Modify module.json of mbed-client module

You need to add your target name to `module.json` so that when you set `yt target <platform>`, yotta can resolve the dependency correctly and link the main library with your module.

Two platforms, mbed and linux, are already supported. You just need to add your module support after that.

```
{
  "name": "mbed-client",
  "version": "0.1.6",
  "description": "Mbed Client API",
  "private": true,
  "keywords": [],
  "author": "Yogesh Pande <yogesh.pande@arm.com>",
  "homepage": "https://github.com/ARMmbed/mbed-client",
  "licenses": [
    {
      "url": "https://spdx.org/licenses/Apache-2.0",
      "type": "Apache-2.0"
    }
  ],
  "dependencies": {
    "mbed-client-c": "~0.1.0"
  },
  "targetDependencies": {
    "arm": {
      "mbed-client-mbed": "~0.0.3"
    },
    "linux": {
      "mbed-client-linux": "~0.0.1"
    },
    "<your platform as defined in target.json>" : {
      "mbed-client-platform": "<published version , can be done later, first link locally as explained in above steps>"
    },
  }
}
```

# Step 5: Testing and verification

You can build your mbed-client port immediately. After that, run:

```
# use the target we previously made locally available (not necessary if your target has been published):
yotta link-target <yourtargetname>
# build!
yotta build
```

A `helloworld-mbedclient` program will be produced inside the `build/<yourtargetname>/test/` directory. This test application may require some changes to compile and run for your platform. First, check for the compilation errors. If you find any, fix the test application for your testing. 

This test is available in `mbed-client/test/helloworld-mbedclient`, and can be found in:

```
build/<yourtargetname>/test/
```

Follow the `readme` instructions of the `mbed-client-linux` example to see what the test application can do.
