## Deprecation note!

**Please note this repository is deprecated - this repository is no longer actively maintained**. 

However, this library is part of Arm's [Mbed Cloud](https://cloud.mbed.com/docs/current) -offering and as such the code is available via [Mbed Cloud Client](https://github.com/ARMmbed/mbed-cloud-client-example) under
[mbed-client](https://github.com/ARMmbed/mbed-cloud-client/tree/master/mbed-client) -folder. That repository is actively maintained.


ARM mbed Client
=======================

This repository contains ARM mbed Client: a library that connects devices to mbed Device Connector Service, mbed Device Server (mDS) and to mbed-enabled cloud services from our partners.

The documentation is collected under the docs directory and the **mbed Client Guide** is also hosted [here](https://docs.mbed.com/docs/mbed-client-guide/en/latest/).

Running Unit Tests
-----------------------

Pre-requisites for the unit tests includes the following tools:
- CppUTest
- XSL
- lcov
- gcovr
- Ninja 

You can use these commands to get the tools if you are running Ubuntu.

    sudo apt-get install cpputest
    sudo apt-get install xsltproc
    sudo apt-get install lcov
    sudo apt-get install gcovr
    sudo apt-get install ninja-build
