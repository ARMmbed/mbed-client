# Introduction to LWM2M mbed client

mbed client is structured as a set of modules. Each module declares which other
modules it depends on. When you build a module, our build system, `yotta`,
looks at these dependencies and installs the necessary modules before
completing the build.

This is also the process that is used to build applications for mbed client
(including the example application in release). The application declares
dependencies on mbed client, and when it is built
`yotta` ensures those modules (any anything that they depend on, recursively)
are present before building. 

In this release you will find that all the modules that each of the
examples depends on have been pre-installed in the `yotta_modules` directory of
each example program, so they do not need to be separately downloaded. In
general, however, `yotta` will download and install the modules that are needed
over the internet from the public yotta Registry (which saves published
versions of modules) or from a specified source control URL.

# Building and Testing mbed client
Before embarking on your own port, you should build the core mbed client for an existing compilation target, to get an understanding of how mbed client builds. You can find a general guide to building mbed client and its example application in the release notes for mbed client.

# Components of mbed client

## Component Software Modules

There is a main component of mbed client. As development continues,
more major components will be added:

 * `lwm2m-client`: the core mbed client, providing C++ APIs for mbed client

This module depends on further internal modules to provide
functionality that it relies on. :

```
lwm2m-client 0.1.6
  ┣─ mbed-client 0.1.1
  ┃ ┗─ mbed-client-libservice 2.0.12
  ┗─ lwm2m-client-linux 0.0.2
```

(You can list dependency trees like this by using the [`yotta list --all`
command](http://docs.yottabuild.org/reference/commands.html)

Note that in this case we've listed the dependencies when the `x86-linux-native`
compilation target was selected. The compilation target can change which
modules are needed. 

If we list the modules contained in the lwm2m client linux example, we can see that
this depends directly only on the `lwm2m-client` and `lwm2m-client-linux` modules, which
each internally depend on various other modules.

```
lwm2m-client-linux-example 0.0.1
┗─ lwm2m-client 0.1.6
  ┣─ mbed-client 0.1.1
  ┃ ┗─ mbed-client-libservice 2.0.12
  ┗─ lwm2m-client-linux 0.0.2
```

## Compilation Targets

This release support one compilation target.

 * `x86-linux-native`: to compile for linux OS, using the gcc toolchain.

To select the compilation target used when compiling a library or example
program, use the [`yotta target x86-linux-native` command]() before buliding. More
documentation about target descriptions can be found on the [yotta
documentation site](http://docs.yottabuild.org/tutorial/targets.html).

# Porting mbed client for different platform

To port mbed client to a new platform, it's necessary to go through the
following steps:

 1. Contact [support@mbed.org](mailto:support@mbed.org), to request the
    creation of new development repositories for your target platform.
 2. Create a yotta compilation target for your board.
 3. Implement the `lwm2m-client-xxx` module for your target platform where `xxx` is the name for your platform like `linux`.
 5. Verify that your implementation is correct
 7. Your port is updated into `module.json` of `lwm2m-client` module.
 8. Your port is can be made available to other customers, and they can
    use your platform to start developing products that leverage your OS!

Our `yotta` build system is designed to make it easy to re-use generic modules.
So if you are intending to support multiple platforms that share common
features, you might choose to split the common functionality into a separate
module, and depend on that module for each platform.


# Step 1: Creating Development Repositories
We provide private git repositories to our partners porting mbed client. These have
an access control list restricted to members of the mbed client team, and relevant
partner contacts and engineers.

When you contact `support@mbed.org` a repository will be created for your
module for a target description for your board:

 * **`lwm2m-client-<platform-name>`** will be the module which provides the `lwm2m-client-xxx`
   implementation for your supported platform. You may
   choose to split it out into further modules in the future, to enable sharing
   of code, but we recommend that you implement the port for your first board
   in this module itself. 
 * **`target-<targetname>`** will contain the yotta target description for the
   target you are porting to mostly your platform name.

# Step 2: Creating a yotta Compilation Target

In order to compile for a target board, you need a [target
description](http://docs.yottabuild.org/tutorial/targets.html) that describes
how to compile for the target. The target description also defines values which
modules can test for when selecting dependency modules. The `lwm2m-client` module,
for example, uses the platform name which each target defines to choose
which `lwm2m-client-<platform-name>` module to depend on to provide the platform
specific implementation.

The target description contains:
 * `target.json` - the target description file
 * a [CMake toolchain
   file](http://www.cmake.org/cmake/help/v3.0/manual/cmake-toolchains.7.html)
   (and any dependencies it needs), which describes how to run the compiler
 * a linker script file

The target description is selected by the user (by running `yotta target
<targetname>`), and used by yotta to set up the build.

yotta's documentation for target descriptions is available on the [yotta
documentation site](http://docs.yottabuild.org/tutorial/targets.html).

Existing examples for compiling for linux target can be found
in the mbed client release: it is cached in the `yotta_targets` directory in
each example application.

The directory structure of a typical target description is:

```
|   readme.md
│   target.json
├───CMake
│   │   toolchain.cmake
│   │
│   ├───Compiler
│   │       GNU-ASM.cmake
│   │       GNU-C.cmake
│   │       GNU-CXX.cmake
│   │
│   └───Platform
│           xxx-GNU-C.cmake
│           xxx-GNU-CXX.cmake
│           xxx.cmake
│
└───ld
        newtarget.ld
```

Where the `target.json` file specifies the path to the CMake toolchain
(`toolchain.cmake`), which uses the standard CMake mechanisms for including
compiler, platform and language specific configuration.

Note that improvements are planned to the yotta target description to allow more re-use of these descriptions between targets.

To make your target available to use locally (without publishing it), you can use `yotta link-target` to “link” it into the globally install targets directory:

```
# in the directory of your target:
yotta link-target
```

You can subsequently use `yotta link-target <targetname>` to make the globally linked target available when compiling another module. After doing that, use `yotta target <targetname>` to select your target for compilation.

# Step 3: Implementing lwm2m-client-xxx
First clone your `lwm2m-client-<your-platform-name>` module and
`lwm2m-client` modules from github.

The `lwm2m-client-<your-platform-name>` module needs to provide a socket and timer implementation for
your target platform. lwm-client-xxx module should have files for the main target like m2mconnectionhandlerimpl, m2mtimerimpl header file.

An example of lwm2m-client-platform core:
```
│   module.json
├───lwm2m-client-platform
│       m2mconnectionhandlerimpl.h
│       m2mtimerimpl.h
│
└───source
        m2mconnectionhandlerimpl.cpp
        m2mtimerimpl.cpp
```

To make your module available to other modules that you want to build, you
need to use [`yotta link`](http://docs.yottabuild.org/reference/commands.html#yotta-link)
to "link" it into the module where you want to test it out.

We will then be able to use `yotta link lwm2m-client-xxx` in another module to make
that other module use your in-development lwm2m-client implementation.

```
# in lwm2m-client, link your module:
yotta link
```

You could also just commit and push your untested code to github, but it’s good to be able to test before committing.

# Step 4: Implementing lwm2m-client-platform
Clone your `mbed-hal-<platform>` module from github. You need to make sure it
you use [`yotta link`](http://docs.yottabuild.org/reference/commands.html#yotta-link)
again, to complete the link from the global links directory from this module:

```
# in lwm2m-client-platform, run:
yotta link lwm2m-client-platform
```

This will cause your in-development `lwm2m-client-platform` implementation that
you previously linked to be used when you compile this module. If we don’t do this linking step, then the last version of `lwm2m-client-platform` , if available, that we committed to github will be used.

The `lwm2m-client-platform` module must provide a platform-specific implementation for lwm2m-client. The API which needs porting are defined in
the `lwm2m-clienty-linux` module which is also provided to you. The header files contain documentation alongside the declaration of each function, where the function is described along with its parameters and return value.

The header files (m2mconnectionhandlerimpl.h and m2mtimerimpl.h) contains all the functions that needs porting and corresponding implementation of your platform.

There are two header files which require porting for your platform.
You can check `lwm2m-client-linux` module available through this package to see how it is done for Linux platform. 


# Step 5: Implementing M2MConnectionHandlerImpl class for your platform

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

You need to take care that some of these functions are asynchronous in nature and some are expecting callback from network like receiving data from socket which needs to be communicated back to the `lwm2m-client` so that the library can act on received data. The callback are called back through Observer class defined here `M2MConnectionObserver` .

The file `m2mconnectionobserver.h` is present in `lwm2m-client`. In order to see how the callback needs to be called check the implementation in `m2mconnectionhandlerimpl.cpp` present in `lwm2m-client-linux`. 

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

# Step 5: Implementing M2MTimerImpl class for your platform

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
The timer API functions are asynchronous in nature and it is expected that whenever timer event is available, it is notified  to the `lwm2m-client` so that the library can act on timer expired signal. The callback are called back through Observer class defined here `M2MTimerObserver` .

The file `m2mtimerobserver.h` is present in `lwm2m-client`. In order to see how the callback needs to be called check the implementation in `m2mtimerimpl.cpp` present in `lwm2m-client-linux`. 

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

# Step 7: Modify module.json of lwm2m-client module

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

# Step 6: Testing & Verification
You should be able to build your lwm2m-client port immediately, 
then run:

```
# use the target we previously made locally available (not necessary if your target has been published):
yotta link-target <yourtargetname>
# build!
yotta build
```

A helloworld-lwm2mclient program will be produced inside the `build/<yourtargetname>/test/` directory. This test application might require some changes to compile and run for your platform. Check fot he compilation errors, if any, and fix the test application for your testing. 

This test is available in `lwm2m-client/test/helloworld-lwm2mclient`, and can be found in:

```
build/<yourtargetname>/test/
```

Follow the readme instructions of lwm2m-client-linux example to see what the test application can do.
