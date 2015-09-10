if(TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(m2mtimerpimpl_mbed
        "../../../../yotta_modules/mbed-client-mbed-os/source/m2mtimerpimpl.cpp"
        "main.cpp"
        "m2mtimerpimpl_mbedtest.cpp"
        "test_m2mtimerpimpl_mbed.cpp"
        "../stub/common_stub.cpp"
)

target_link_libraries(m2mtimerpimpl_mbed
    mbed-client
    CppUTest
    CppUTestExt
)

add_test(m2mtimerpimpl_mbed m2mtimerpimpl_mbed)

endif()
