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
    CppUTest
    CppUTestExt
)
set_target_properties(m2mtimerpimpl_mbed m2mtimerpimpl_mbed
PROPERTIES COMPILE_FLAGS "${UTEST_COMPILE_FLAGS}"
           LINK_FLAGS "${UTEST_LINK_FLAGS}")

set(binary "m2mtimerpimpl_mbed -ojunit")
add_test(m2mtimerpimpl_mbed ${binary})

endif()
