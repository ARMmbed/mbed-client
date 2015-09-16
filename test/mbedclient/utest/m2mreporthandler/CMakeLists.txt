if(TARGET_LIKE_LINUX)
include("../includes.txt")


add_executable(m2mreporthandler
        "main.cpp"
        "m2mreporthandlertest.cpp"
        "test_m2mreporthandler.cpp"
        "../stub/m2mtimer_stub.cpp"
        "../stub/m2mstring_stub.cpp"
        "../../../../source/m2mreporthandler.cpp"
        "../../../../source/m2mconstants.cpp"

)
target_link_libraries(m2mreporthandler
    CppUTest
    CppUTestExt
)

set_target_properties(m2mreporthandler
PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage -fprofile-arcs -ftest-coverage -D__thumb2__ -w"
           LINK_FLAGS "-fprofile-arcs")

add_test(m2mreporthandler m2mreporthandler)

endif()
