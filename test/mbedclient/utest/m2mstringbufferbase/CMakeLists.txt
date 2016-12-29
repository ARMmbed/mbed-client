if(TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(m2mstringbufferbase 
	"main.cpp"
	"m2mstringbufferbasetest.cpp"
	"test_m2mstringbufferbase.cpp"
        "../stub/m2mstring_stub.cpp"
        "../../../../source/m2mstringbufferbase.cpp"
)
target_link_libraries(m2mstringbufferbase
    CppUTest
    CppUTestExt
)
set_target_properties(m2mstringbufferbase
PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS}"
           LINK_FLAGS "${UTEST_LINK_FLAGS}")
set(binary "m2mstringbufferbase")
add_test(m2mstringbufferbase ${binary})


endif()
