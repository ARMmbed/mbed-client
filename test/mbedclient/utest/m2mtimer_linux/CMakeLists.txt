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
    mbed-client
    CppUTest
    CppUTestExt
)

add_test(m2mtimer_linux m2mtimer_linux)

endif()
