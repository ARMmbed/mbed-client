if(TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(m2mserver
        "main.cpp"
        "m2mservertest.cpp"
        "test_m2mserver.cpp"
        "../stub/m2mobject_stub.cpp"
        "../stub/m2mstring_stub.cpp"
        "../stub/m2mobjectinstance_stub.cpp"
        "../stub/m2mresource_stub.cpp"
        "../stub/m2mresourceinstance_stub.cpp"
        "../stub/m2mbase_stub.cpp"
        "../../../../source/m2mserver.cpp"
        "../../../../source/m2mconstants.cpp"
)

target_link_libraries(m2mserver
    mbed-client
    CppUTest
    CppUTestExt
)

add_test(m2mserver m2mserver)

endif()
