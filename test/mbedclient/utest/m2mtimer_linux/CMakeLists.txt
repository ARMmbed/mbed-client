if(TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(m2mtimer_linux
        "../../../../yotta_modules/mbed-client-linux/source/m2mtimer.cpp"
        "main.cpp"
        "test_m2mtimer.cpp"
        "m2mtimertest.cpp"
        "../stub/m2mtimerpimpl_stub.cpp"
        "../stub/threadhelper_stub.cpp"
)

target_link_libraries(m2mtimer_linux
    CppUTest
    CppUTestExt
)
set_target_properties(m2mtimer_linux
PROPERTIES COMPILE_FLAGS "${UTEST_COMPILE_FLAGS}"
           LINK_FLAGS "${UTEST_LINK_FLAGS}")

set(binary "m2mtimer_linux -ojunit")
add_test(m2mtimer_linux ${binary})

endif()
