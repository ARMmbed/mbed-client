if(!TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(m2mresourceinstance
        "../../../../source/m2mresourceinstance.cpp"
	"main.cpp"
        "../stub/m2mbase_stub.cpp"
        "../stub/m2mstring_stub.cpp"
        "../stub/m2mtimer_stub.cpp"
        "../stub/m2mreporthandler_stub.cpp"
        "../stub/common_stub.cpp"
        "../stub/m2mtlvdeserializer_stub.cpp"
	"m2mresourceinstancetest.cpp"
        "test_m2mresourceinstance.cpp"
)
target_link_libraries(m2mresourceinstance
    CppUTest
    CppUTestExt
)

add_test(m2mresourceinstance m2mresourceinstance)

endif()
