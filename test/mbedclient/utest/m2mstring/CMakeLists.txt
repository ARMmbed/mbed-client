if(TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(m2mstring
        "main.cpp"
        "test_m2mstring.cpp"
        "m2mstringtest.cpp"
        "../../../../source/m2mstring.cpp"
)

target_link_libraries(m2mstring
    CppUTest
    CppUTestExt
)
set_target_properties(m2mstring
PROPERTIES COMPILE_FLAGS "${UTEST_COMPILE_FLAGS}"
           LINK_FLAGS "${UTEST_LINK_FLAGS}")
add_test(m2mstring m2mstring)

endif()
