# Introduction to LWM2M mbed client

mbed client is structured as a set of modules. Each module declares which other
modules it depends on. When you build a module, our build system `yotta`
looks at these dependencies and installs the necessary modules before
completing the build.

This is also the process to build applications for the mbed client
(including the example application in the release). The application declares
dependencies on mbed client, and when it is built
`yotta` ensures that the modules (and anything that they depend on, recursively)
are present before building. 

In this release, all the necessary modules for the examples (the ones that the
examples depend on) have been pre-installed in the `yotta_modules` directory of
each example program, so you do not need to download them separately. In
general, `yotta` downloads and installs the necessary modules over the internet from the public yotta Registry 
(which saves published versions of modules) or from a specified source control URL.

# Building and Testing mbed client

Before embarking on your own port, you should build the core mbed client for an existing compilation target to get an understanding of how the mbed client builds. You can find a general guide for building the mbed client and its example application in the release notes for mbed client.

# Components of mbed Client

## Component Software Modules

The mbed Client consists of one main component. More major components will be added in the further development phases:

 * `lwm2m-client` is the core mbed Client, providing C++ APIs for the mbed client

This module depends on further internal modules:

```
lwm2m-client 0.1.6
|
|_mbed-client 0.1.1
|   |_mbed-client-libservice 2.0.12
|
|_lwm2m-client-linux 0.0.2
```

To list the dependency trees, give [`yotta list --all`
command](http://docs.yottabuild.org/reference/commands.html).

**Note**: In this case we have listed the dependencies for the `x86-linux-native`
compilation target. Different modules are needed for different compilation target.

If you list the modules contained in the LWM2M client linux example, you can see that
it depends directly only on the `lwm2m-client` and `lwm2m-client-linux` modules. These modules depend internally on various other modules.

```
lwm2m-client-linux-example 0.0.1
|
|_lwm2m-client 0.1.6
   |
   |_mbed-client 0.1.1
   |   |_mbed-client-libservice 2.0.12
   |
   |_lwm2m-client-linux 0.0.2
```

## Compilation Targets

This release supports only one compilation target:

 * `x86-linux-native`: to compile for linux OS, using the gcc toolchain.

To select the compilation target used for compiling a library or example
program, give the command `yotta target x86-linux-native` before buliding. More
documentation on target descriptions can be found on the [yotta
documentation site](http://docs.yottabuild.org/tutorial/targets.html).

# Porting mbed Client to different platform

To port mbed Client to a new platform, perform the following steps:

 1. Contact [support@mbed.org](mailto:support@mbed.org), to request for new development repositories for your target platform.
 2. Create a yotta compilation target for your board.
 3. Implement the `lwm2m-client-xxx` module for your target platform where `xxx` is the name for your platform (for example, `linux`).
 4. Make sure that your port is updated into `module.json` of the `lwm2m-client` module.
 5. Verify that your implementation is correct.

The `yotta` build system is designed for easy reuse of generic modules.
If you intend to support multiple platforms that share common features, it is recommended to store the common 
functionality into a separate module and use it for each platform.


# Step 1: Creating Development Repositories
We provide private git repositories to our partners porting mbed Client. Only the members of the mbed Client team, and relevant partner contacts and engineers have access to these repositories.

When you contact `support@mbed.org`, a repository will be created for your
module. You also need to provide the target description of your board:

- **`lwm2m-client-<platform-name>`** is the module that provides the `lwm2m-client-xxx`
   implementation for your platform. You may choose to split it into further modules in the future, to enable sharing
   of code, but we recommend that you implement the port for your first board in this module itself. 

- **`target-<targetname>`** contains the yotta target description of the
   target you are porting to. This is usually your platform name.

# Step 2: Creating a yotta Compilation Target

To compile for a target board, you need a [target description](http://docs.yottabuild.org/tutorial/targets.html) that describes how to compile for the target. 

**Pekka says this is confusing:**
**Yogesh : Updated , Comments ??**
The `lwm2m-client` module uses the platform name, which each target defines, to choose which `lwm2m-client-<platform-name>` module to depend on to provide the platform
specific implementation.
**Until here**

The target description contains:
 * `target.json` - the target description file
 * a [CMake toolchain
   file](http://www.cmake.org/cmake/help/v3.0/manual/cmake-toolchains.7.html)
   (and any dependencies it needs), that describes how to run the compiler
 * a linker script file

To select the target description, run `yotta target <targetname>`. yotta needs this to set up the build.

The documentation for target descriptions is available on the [yotta
documentation site](http://docs.yottabuild.org/tutorial/targets.html).

Existing example for compiling for linux target can be found in the `yotta_targets` directory in example application.

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

Where the `target.json` file specifies the path to the CMake toolchain (`toolchain.cmake`), that uses the standard CMake mechanisms for including compiler, platform and language specific configuration.

**Note**: Improvements are planned to the yotta target description to allow more reuse of the descriptions between targets.

To make your target available locally (without publishing it), you can give `yotta link-target` command to “link” it into the global install targets directory:

```
# in the directory of your target:
yotta link-target
```

You can then give `yotta link-target <targetname>` command to make the globally linked target available when compiling another module. After that, give `yotta target <targetname>` command to select your target for the compilation.

# Step 3: Implementing lwm2m-client-xxx

First, clone your `lwm2m-client-<your-platform-name>` module and `lwm2m-client` modules from github.

The `lwm2m-client-<your-platform-name>` module needs to provide a socket and timer implementation for
your target platform. The lwm-client-xxx module should include files for the main target, such as `m2mconnectionhandlerimpl`and `m2mtimerimpl` header files.

An example of lwm2m-client-platform core:
```
|_module.json
|
|_lwm2m-client-platform
|    |_m2mconnectionhandlerimpl.h
|    |_m2mtimerimpl.h
|
|_source
    |_m2mconnectionhandlerimpl.cpp
    |_m2mtimerimpl.cpp
```

To make your module available to other modules that you want to build, you
need to give [`yotta link`](http://docs.yottabuild.org/reference/commands.html#yotta-link) command
to "link" it into the module where you want to test it out.

**Pekka says: This could be written more clearly. Who gives that command (or are these yotta things commands?) where and what is then available for whom?**
**Yogesh : Text is now updated, Comments ??**
You will then be able to use your local your in-development lwm2m-client implementation by giving command `yotta link lwm2m-client-xxx` in main `lwm2m-client` module so that it starts using 
your local module.

```
# in lwm2m-client, link your module:
yotta link
```

You can also just commit and push your untested code to github, but it is always a good idea to test before committing.

Your `lwm2m-client-xxx` module must provide a platform-specific implementation for the lwm2m-client. The APIs that needs porting are defined in the `lwm2m-client-linux` module. The header files contain documentation alongside the declaration of each function, where the function is described along with its parameters and return value.

**Pekka says: Is this redundant. You just named above two header files that require porting.**
**Yogesh : Updated, Comments ??**
There are two header files which require porting for your platform.
1. `m2mconnectionhandlerimpl.h`
2. `m2mtimerimpl.h` 

To see how this is done in Linux, check the `lwm2m-client-linux` module available in this package.

## Implementing M2MConnectionHandlerImpl class for your platform

```
/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_CONNECTION_HANDLER_IMPL_H__
#define M2M_CONNECTION_HANDLER_IMPL_H__

#include "lwm2m-client/m2mconfig.h"
#include "lwm2m-client/m2minterface.h"
#include "lwm2m-client/m2mconnectionobserver.h"
#include "nsdl-c/sn_nsdl.h"

/**
 * @brief M2MConnectionHandler.
 * This class handles the socket connection for LWM2M Client
 */

class M2MConnectionHandlerImpl {

public:

    /**
    * @brief Constructor
    */
    M2MConnectionHandlerImpl(M2MConnectionObserver &observer,
                             M2MInterface::NetworkStack stack);

    /**
    * @brief Destructor
    */
    ~M2MConnectionHandlerImpl();

    /**
    * @brief This binds the socket connection.
    * @param listen_port Port to listen for incoming connection.
    * @return true if successfulelse false.
    */
    bool bind_connection(const uint16_t listen_port);

    /**
    * @brief This resolves the server address. Output is
    * returned through callback
    * @param String server address.
    * @param uint16_t Server port.
    * @param ServerType, Server Type to be resolved.
    * @return true if address is valid else false.
    */
    bool resolve_server_address(const String& server_address,
                                const uint16_t server_port,
                                M2MConnectionObserver::ServerType server_type);

    /**
    * @brief Sends data, to the connected sent to server.
    * @param data, Data to be sent.
    */
    bool send_data(uint8_t *data_ptr,
                   uint16_t data_len,
                   sn_nsdl_addr_s *address_ptr);

    /**
    * @brief Returns server port which is set.
    * @return Port,Server port
    */
    uint16_t server_port();

    /**
    * @brief Returns listening port which is set.
    * @return Port,Listening port
    */
    uint16_t listen_port();

    /**
    * @brief Listens for incoming data from remote server
    * @return true if successful else false.
    */
    bool listen_for_data();

    void data_receive(void *object);

};
#endif //M2M_CONNECTION_HANDLER_IMPL_H__

```

You need to make sure that some of these functions are asynchronous in nature and some are expecting callback from the network. For example, receiving data from socket needs to be communicated back to the `lwm2m-client` so that the library can act on the data received. The callback comes through the Observer class defined in `M2MConnectionObserver`.

The file `m2mconnectionobserver.h` is present in the `lwm2m-client`. To see how the callback needs to be called, check the implementation in `m2mconnectionhandlerimpl.cpp` present in the `lwm2m-client-linux`. 

```
/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_CONNECTION_OBSERVER_H__
#define M2M_CONNECTION_OBSERVER_H__

#include "lwm2m-client/m2minterface.h"

/**
 * @brief Observer class for informing socket activity to the state machine
 */

class M2MConnectionObserver
{

public :

    typedef enum {
        Bootstrap,
        LWM2MServer
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

## Implementing M2MTimerImpl class for your platform

```
/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_TIMER_IMPL_H
#define M2M_TIMER_IMPL_H

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
public:

    // Prevents the use of assignment operator
    M2MTimerImpl& operator=(const M2MTimerImpl& other);

    // Prevents the use of copy constructor
    M2MTimerImpl(const M2MTimerImpl& other);

    /**
    * Constructor.
    */
    M2MTimerImpl(M2MTimerObserver& _observer);

    /**
    * Destructor.
    */
    virtual ~M2MTimerImpl();

    /**
     * Starts timer
     * @param interval Timer's interval in milliseconds
    * @param single_shot defines if timer is ticked
    * once or is it restarted everytime timer is expired.
    */
    void start_timer(uint64_t interval, bool single_shot = true);

    /**
    * Stops timer.
    * This cancels the ongoing timer.
    */
    void stop_timer();

    /**
    * Callback function for timer completion.
    */
    void timer_expired();

};

#endif // M2M_TIMER_IMPL_H
```

**Pekka says: Can you say callback is called back? This is confusing, at least for me. Callback is received/sent/something else?**
**Yogesh : Updated, Comments ??**
The timer API functions are asynchronous in nature and it is expected that whenever a timer event is available, it is notified to the `lwm2m-client` so that the library can act on the "timer expired" signal. The callback is received through an Observer class defined in `M2MTimerObserver` .

The file `m2mtimerobserver.h` is present in `lwm2m-client`. To see how the callback needs to be called check the implementation in `m2mtimerimpl.cpp` present in  the`lwm2m-client-linux`. 

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
        PMaxTimer
    }Type;

    /**
    * Indicates that the time has expired.
    */
    virtual void timer_expired(M2MTimerObserver::Type type =
                               M2MTimerObserver::Notdefined) = 0;
};

#endif // M2M_TIMER_OBSERVER_H
```

# Step 4: Modify module.json of lwm2m-client module

You need to add your target name to module.json so that when you set `yt target <platform>` , yotta can resolve the dependency correctly and link the main library with your module.

As you can see ,there is already support available for 2 platform , mbed and linux, you just need to add your module support after that
```
{
  "name": "lwm2m-client",
  "version": "0.1.6",
  "description": "LWM2M mbed Client API",
  "private": true,
  "keywords": [],
  "author": "Yogesh Pande <yogesh.pande@arm.com>",
  "homepage": "https://github.com/ARMmbed/lwm2m-client",
  "licenses": [
    {
      "url": "https://spdx.org/licenses/Apache-2.0",
      "type": "Apache-2.0"
    }
  ],
  "dependencies": {
    "mbed-client": "~0.1.0"
  },
  "targetDependencies": {
    "arm": {
      "lwm2m-client-mbed": "~0.0.3"
    },
    "linux": {
      "lwm2m-client-linux": "~0.0.1"
    },
    "<your platform as defined in target.json>" : {
      "lwm2m-client-platform": "<published version , can be done later, first link locally as explained in above steps>"
    },
  }
}
```

# Step 5: Testing & Verification

You should be able to build your lwm2m-client port immediately. After that, run:

```
# use the target we previously made locally available (not necessary if your target has been published):
yotta link-target <yourtargetname>
# build!
yotta build
```

A helloworld-lwm2mclient program will be produced inside the `build/<yourtargetname>/test/` directory. This test application might require some changes to compile and run for your platform. First, check for the compilation errors. If you find any, fix the test application for your testing. 

This test is available in `lwm2m-client/test/helloworld-lwm2mclient`, and can be found in:

```
build/<yourtargetname>/test/
```

Follow the readme instructions of the lwm2m-client-linux example to see what the test application can do.
