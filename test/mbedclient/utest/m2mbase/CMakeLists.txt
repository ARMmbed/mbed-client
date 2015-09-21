if(TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(m2mbase 
	"../../../../source/m2mbase.cpp"
	"../../../../source/m2mconstants.cpp"
	"main.cpp"
        "../stub/m2mreporthandler_stub.cpp"
        "../stub/m2mstring_stub.cpp"
	"m2mbasetest.cpp"
	"test_m2mbase.cpp"
)
target_link_libraries(m2mbase
    CppUTest
    CppUTestExt
)
set_target_properties(m2mbase
PROPERTIES COMPILE_FLAGS "${UTEST_COMPILE_FLAGS}"
           LINK_FLAGS "${UTEST_LINK_FLAGS}")
add_test(m2mbase m2mbase)


endif()
