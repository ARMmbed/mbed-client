if(TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(m2mstring
        "main.cpp"
        "test_m2mstring.cpp"
        "m2mstringtest.cpp"
        "../../../../source/m2mstring.cpp"
)

target_link_libraries(m2mstring
    mbed-client
    CppUTest
    CppUTestExt
)
set_target_properties(m2mstring
PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage -fprofile-arcs -ftest-coverage -D__thumb2__ -w"
           LINK_FLAGS "-fprofile-arcs")
add_test(m2mstring m2mstring)

endif()
