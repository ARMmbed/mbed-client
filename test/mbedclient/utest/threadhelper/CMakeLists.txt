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
PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage -fprofile-arcs -ftest-coverage -D__thumb2__ -w"
           LINK_FLAGS "-fprofile-arcs")
add_test(threadhelper threadhelper)
endif()
