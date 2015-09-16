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
    CppUTest
    CppUTestExt
)
set_target_properties(nsdlaccesshelper
PROPERTIES COMPILE_FLAGS "${UTEST_COMPILE_FLAGS}"
           LINK_FLAGS "${UTEST_LINK_FLAGS}")
add_test(nsdlaccesshelper nsdlaccesshelper)


endif()
