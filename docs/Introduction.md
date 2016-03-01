# ARM mbed Client overview

ARM mbed Client is a library that provides the means to connect constrained embedded devices to mbed Device Connector Service, mbed Device Server and to mbed-enabled cloud services from our partners.

The mbed Client high-level APIs allow mbed OS developers to create applications with LWM2M features as described in the [Lightweight Machine to Machine Technical Specification](http://technical.openmobilealliance.org/Technical/technical-information/release-program/current-releases/oma-lightweightm2m-v1-0):

- Manage devices on mbed Device Server.
- Securely communicate with internet services over the industry standard TLS/DTLS.
- Fully control the endpoint and application logic. 

The API is written in C++ to allow quick application development.

## Managing devices on mbed Device Server

mbed Client supports the following three features introduced in the subsequent chapters:

- [Client Registration and Deregistration](client_reg_dereg.md)
- [Device Management and Service Enablement](dev_man_serv_enable.md)
- [Information Reporting](info_reporting.md)

The API also provides an interface to define the application endpoint information. This information will be delivered to mbed Device Server during the registration operation.

First, you need to create an interface for mbed Client:

```
#include "mbed-client/m2minterfacefactory.h"
#include "mbed-client/m2minterface.h"

M2MInterface* interface = M2MInterfaceFactory::create_interface(*this,
                                                  "mbed-endpoint",
                                                  "test",
                                                  3600,
                                                  5684,
                                                  "",
                                                  M2MInterface::UDP,
                                                  M2MInterface::LwIP_IPv4,
                                                  "");
```

### Maximum UDP message size

The maximum single UDP message size that mbed Client can receive is 1152 bytes. The actual payload size is 1137 bytes, the header information using the remaining 15 bytes. 

For transferring larger amounts of data, the Blockwise feature must be deployed. When using this feature, mbed Client can handle messages up to 64KB. This feature is disabled by default.

To enable the Blockwise feature, you need to create a `config.json` file in the application level. 

An example:
```
{
"coap_max_blockwise_payload_size": 1024,
}
```

Acceptable values for the `coap_max_blockwise_payload_size` flag are:
0, 16, 32, 64, 128, 256, 512 and 1024. Value 0 means that the feature is not used.

## How to use the API
More information on how to use the API effectively to create and configure Objects, Object Instances and Resources, can be found [here](howto.md).

## API documentation

The documentation for this API is [available here](https://docs.mbed.com/docs/mbed-client-guide/en/latest/api/annotated.html).

## Example application

We have an example application for

1. [mbed OS](https://github.com/ARMmbed/mbed-client-examples).

2. [Ubuntu](https://github.com/ARMmbed/mbed-client-linux-example).

