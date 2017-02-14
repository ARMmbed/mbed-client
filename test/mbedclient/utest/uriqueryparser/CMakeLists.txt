if(TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(uriqueryparser
        "../../../../source/uriqueryparser.cpp"
        "main.cpp"
        "uriqueryparsertest.cpp"
        "test_uriqueryparser.cpp"
)

target_link_libraries(uriqueryparser
    CppUTest
    CppUTestExt
)
set_target_properties(uriqueryparser
PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS}"
           LINK_FLAGS "${UTEST_LINK_FLAGS}")

set(binary "uriqueryparser")
add_test(uriqueryparser ${binary})

endif()
