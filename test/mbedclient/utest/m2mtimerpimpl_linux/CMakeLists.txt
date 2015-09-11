if(TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(m2mtimerpimpl_linux
        "../../../../yotta_modules/mbed-client-linux/source/m2mtimerpimpl.cpp"
        "main.cpp"
        "m2mtimerpimpl_linuxtest.cpp"
        "test_m2mtimerpimpl_linux.cpp"
        "../stub/common_stub.cpp"
        "../stub/m2mstring_stub.cpp"
        "../stub/threadhelper_stub.cpp"
)

target_link_libraries(m2mtimerpimpl_linux
    mbed-client
    CppUTest
    CppUTestExt
)
set_target_properties(m2mtimerpimpl_linux
PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage -fprofile-arcs -ftest-coverage -D__thumb2__ -w"
           LINK_FLAGS "-fprofile-arcs")
add_test(m2mtimerpimpl_linux m2mtimerpimpl_linux)

endif()
