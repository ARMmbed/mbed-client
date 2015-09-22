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
    CppUTest
    CppUTestExt
)
set_target_properties(threadhelper
PROPERTIES COMPILE_FLAGS "${UTEST_COMPILE_FLAGS}"
           LINK_FLAGS "${UTEST_LINK_FLAGS}")

set(binary "threadhelper -ojunit")
add_test(threadhelper ${binary})
endif()
