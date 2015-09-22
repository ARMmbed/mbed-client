if(TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(m2mconnectionhandlerpimpl_mbed
        "../../../../yotta_modules/mbed-client-mbed-os/source/m2mconnectionhandlerpimpl.cpp"
        "main.cpp"
        "m2mconnectionhandlerpimpl_mbedtest.cpp"
        "test_m2mconnectionhandlerpimpl_mbed.cpp"
        "../stub/m2mconnectionhandler_stub.cpp"
        "../stub/common_stub.cpp"
        "../stub/nsdlaccesshelper_stub.cpp"
        "../stub/m2mstring_stub.cpp"
        "../stub/m2mconnectionsecurity_stub.cpp"
        "../stub/m2msecurity_stub.cpp"
        "../stub/m2mbase_stub.cpp"
        "../stub/m2mobject_stub.cpp"
)
target_link_libraries(m2mconnectionhandlerpimpl_mbed
    CppUTest
    CppUTestExt
)
set_target_properties(m2mconnectionhandlerpimpl_mbed
PROPERTIES COMPILE_FLAGS "${UTEST_COMPILE_FLAGS}"
           LINK_FLAGS "${UTEST_LINK_FLAGS}")
set(binary "m2mconnectionhandlerpimpl_mbed -ojunit")
add_test(m2mconnectionhandlerpimpl_mbed ${binary})

endif()
