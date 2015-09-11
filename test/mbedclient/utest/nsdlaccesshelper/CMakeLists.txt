if(TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(nsdlaccesshelper
        "../../../../source/nsdlaccesshelper.cpp"
        "main.cpp"
        "nsdlaccesshelpertest.cpp"
        "test_nsdlaccesshelper.cpp"
        "../stub/common_stub.cpp"
        "../stub/m2mnsdlinterface_stub.cpp"
)

target_link_libraries(nsdlaccesshelper
    mbed-client
    CppUTest
    CppUTestExt
)
set_target_properties(nsdlaccesshelper
PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage -fprofile-arcs -ftest-coverage -D__thumb2__ -w"
           LINK_FLAGS "-fprofile-arcs")
add_test(nsdlaccesshelper nsdlaccesshelper)


endif()
