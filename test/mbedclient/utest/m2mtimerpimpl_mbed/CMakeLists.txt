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
PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage -fprofile-arcs -ftest-coverage -D__thumb2__ -w"
           LINK_FLAGS "-fprofile-arcs")
add_test(m2mtimerpimpl_mbed m2mtimerpimpl_mbed)

endif()
