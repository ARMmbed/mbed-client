if(!TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(m2mresource
        "../../../../source/m2mresource.cpp"
	"main.cpp"
        "../stub/m2mbase_stub.cpp"
        "../stub/m2mstring_stub.cpp"
        "../stub/m2mtimer_stub.cpp"
        "../stub/m2mreporthandler_stub.cpp"
        "../stub/m2mresourceinstance_stub.cpp"
	"m2mresourcetest.cpp"
        "test_m2mresource.cpp"
)
target_link_libraries(m2mresource
    CppUTest
    CppUTestExt
)

add_test(m2mresource m2mresource)

endif()
