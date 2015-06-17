if(TARGET_LIKE_LINUX)
add_executable(lwm2m-client-test-lwm2mclient_linux 
        "lwm2mclient_linux/main.cpp"
)
target_link_libraries(lwm2m-client-test-lwm2mclient_linux
    mbed-client
    lwm2m-client-linux
    lwm2m-client
)
add_test(lwm2m-client-test-lwm2mclient_linux lwm2m-client-test-lwm2mclient_linux)
add_dependencies(all_tests lwm2m-client-test-lwm2mclient_linux)

add_executable(lwm2m-client-test-helloworld-lwm2mclient 
        "helloworld-lwm2mclient/main.cpp"
        "helloworld-lwm2mclient/to_be_ported.c"
)
target_link_libraries(lwm2m-client-test-helloworld-lwm2mclient
    mbed-client
    lwm2m-client-linux
    lwm2m-client
)
add_test(lwm2m-client-test-helloworld-lwm2mclient lwm2m-client-test-helloworld-lwm2mclient)
add_dependencies(all_tests lwm2m-client-test-helloworld-lwm2mclient)

endif()

if(TARGET_LIKE_MBED)
add_executable(lwm2m-client-test-lwm2mclient-smokeTest 
        "lwm2mclient-smokeTest/main.cpp"
        "lwm2mclient-smokeTest/testconfig.cpp"
        "lwm2mclient-smokeTest/testconfig.h"
)
target_link_libraries(lwm2m-client-test-lwm2mclient-smokeTest
    mbed-client
    lwm2m-client-mbed
    lwm2m-client
)
add_test(lwm2m-client-test-lwm2mclient-smokeTest lwm2m-client-test-lwm2mclient-smokeTest)
add_dependencies(all_tests lwm2m-client-test-lwm2mclient-smokeTest)

add_executable(lwm2m-client-test-helloworld-lwm2mclient 
        "helloworld-lwm2mclient/main.cpp"
        "helloworld-lwm2mclient/to_be_ported.c"
)
target_link_libraries(lwm2m-client-test-helloworld-lwm2mclient
    mbed-client
    lwm2m-client-mbed
    lwm2m-client
)
add_test(lwm2m-client-test-helloworld-lwm2mclient lwm2m-client-test-helloworld-lwm2mclient)
add_dependencies(all_tests lwm2m-client-test-helloworld-lwm2mclient)

endif()


