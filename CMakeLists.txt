INCLUDE(CMakeForceCompiler)
# CROSS COMPILER SETTING
cmake_minimum_required (VERSION 2.8)
SET(CMAKE_SYSTEM_NAME Generic)

project(mbedClient)

set(LINUX_PORT_LIBS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../Linux_port_libs/)
#own include dirs
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-classic/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-mbed-tls/)



#pal include Dirs
set(PAL_APIS_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../mbed-client-pal/Source/PAL-Impl/Services-API/)
include_directories(${PAL_APIS_INCLUDE_PATH}/)  
SET(PAL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../mbed-client-pal/Source)
#need to make this platform
include_directories(${PAL_SOURCE_DIR}/Port/Reference-Impl/Linux/CFStore)
include_directories(${PAL_SOURCE_DIR}/Port/Platform-API/)

#porting libs
include_directories(${LINUX_PORT_LIBS_PATH}/nanostack-libservice/mbed-client-libservice)
include_directories(${LINUX_PORT_LIBS_PATH}/nanostack-libservice/)    
include_directories(${LINUX_PORT_LIBS_PATH}/sal-stack-nanostack-eventloop/nanostack-event-loop)  
include_directories(${LINUX_PORT_LIBS_PATH}/ns-hal-pal/)
include_directories(${LINUX_PORT_LIBS_PATH}/mbed-client-c/)
include_directories(${LINUX_PORT_LIBS_PATH}/mbed-trace/)       

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h????????????????????????????????????????????????????????????????????????????


FILE(GLOB MBED_CLIENT_SRC
	"${CMAKE_CURRENT_SOURCE_DIR}/mbed-client/*.h"
	
	"${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-classic/mbed-client-classic/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-classic/source/*.cpp"

	"${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-mbed-tls/mbed-client-mbed-tls/*.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-mbed-tls/source/*.cpp"
	
	"${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/include/*.h"
	)

CREATE_LIBRARY(mbedClient "${MBED_CLIENT_SRC}" "")
	
	
	




