if(TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(m2mconnectionhandler_mbed
        "../../../../yotta_modules/mbed-client-mbed-os/source/m2mconnectionhandler.cpp"
        "main.cpp"
        "m2mconnectionhandler_mbedtest.cpp"
        "test_m2mconnectionhandler_mbed.cpp"
        "../stub/common_stub.cpp"
        "../stub/nsdlaccesshelper_stub.cpp"
        "../stub/m2mstring_stub.cpp"
        "../stub/m2mconnectionsecurity_stub.cpp"
        "../stub/m2mconnectionhandlerpimpl_stub.cpp"
)
target_link_libraries(m2mconnectionhandler_mbed
    CppUTest
    CppUTestExt
)
set_target_properties(m2mconnectionhandler_mbed
PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage -fprofile-arcs -ftest-coverage -D__thumb2__ -w"
           LINK_FLAGS "-fprofile-arcs")
add_test(m2mconnectionhandler_mbed m2mconnectionhandler_mbed)
endif()
