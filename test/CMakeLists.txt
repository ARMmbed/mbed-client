if(TARGET_LIKE_LINUX)
add_executable(mbed-client-test-mbedclient_linux 
        "mbedclient_linux/main.cpp"
)
target_link_libraries(mbed-client-test-mbedclient_linux
    mbed-client-c
    mbed-client-linux
    mbed-client
)
add_test(mbed-client-test-mbedclient_linux mbed-client-test-mbedclient_linux)
add_dependencies(all_tests mbed-client-test-mbedclient_linux)

add_executable(mbed-client-test-helloworld-mbedclient 
        "helloworld-mbedclient/main.cpp"
        "helloworld-mbedclient/mbedclient.cpp"
        "helloworld-mbedclient/to_be_ported.c"
)
target_link_libraries(mbed-client-test-helloworld-mbedclient
    mbed-client-c
    mbed-client-linux
    mbed-client
)
add_test(mbed-client-test-helloworld-mbedclient mbed-client-test-helloworld-mbedclient)
add_dependencies(all_tests mbed-client-test-helloworld-mbedclient)

endif()

if(TARGET_LIKE_MBED)

add_executable(mbed-client-test-mbedclient-smokeTest 
        "mbedclient-smokeTest/main.cpp"
        "mbedclient-smokeTest/testconfig.cpp"
        "mbedclient-smokeTest/testconfig.h"
)
target_link_libraries(mbed-client-test-mbedclient-smokeTest
    mbed-client-c
    mbed-client-mbed-os
    mbed-client
)

add_test(mbed-client-test-mbedclient-smokeTest mbed-client-test-mbedclient-smokeTest)
add_dependencies(all_tests mbed-client-test-mbedclient-smokeTest)

# if the target has defined a post-processing step, perform it:
if(YOTTA_POSTPROCESS_COMMAND)
    string(REPLACE YOTTA_CURRENT_EXE_NAME "mbed-client-test-mbedclient-smokeTest" LOCAL_POSTPROCESS_COMMAND "${YOTTA_POSTPROCESS_COMMAND}")
    separate_arguments(LOCAL_POSTPROCESS_COMMAND_SEPARATED UNIX_COMMAND ${LOCAL_POSTPROCESS_COMMAND})
    add_custom_command(
        TARGET mbed-client-test-mbedclient-smokeTest
        POST_BUILD
        COMMAND ${LOCAL_POSTPROCESS_COMMAND_SEPARATED}
    )
endif()

add_executable(mbed-client-test-helloworld-mbedclient 
        "helloworld-mbedclient/main.cpp"
        "helloworld-mbedclient/mbedclient.cpp"
        "helloworld-mbedclient/to_be_ported.c"
)
target_link_libraries(mbed-client-test-helloworld-mbedclient
    mbed-client-c
    mbed-client-mbed-os
    mbed-client
)
# if the target has defined a post-processing step, perform it:
if(YOTTA_POSTPROCESS_COMMAND)
    string(REPLACE YOTTA_CURRENT_EXE_NAME "mbed-client-test-helloworld-mbedclient" LOCAL_POSTPROCESS_COMMAND "${YOTTA_POSTPROCESS_COMMAND}")
    separate_arguments(LOCAL_POSTPROCESS_COMMAND_SEPARATED UNIX_COMMAND ${LOCAL_POSTPROCESS_COMMAND})
    add_custom_command(
        TARGET mbed-client-test-helloworld-mbedclient
        POST_BUILD
        COMMAND ${LOCAL_POSTPROCESS_COMMAND_SEPARATED}
    )
endif()

add_test(mbed-client-test-helloworld-mbedclient mbed-client-test-helloworld-mbedclient)
add_dependencies(all_tests mbed-client-test-helloworld-mbedclient)

endif()


