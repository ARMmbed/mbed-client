if(TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(threadhelper
        "../../../../yotta_modules/mbed-client-linux/source/threadhelper.cpp"
        "main.cpp"
        "threadhelpertest.cpp"
        "test_threadhelper.cpp"
        "../stub/common_stub.cpp"
)

target_link_libraries(threadhelper
    mbed-client
    CppUTest
    CppUTestExt
)

add_test(threadhelper threadhelper)
endif()
