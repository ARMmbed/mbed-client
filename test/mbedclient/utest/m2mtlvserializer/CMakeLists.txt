if(TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(m2mtlvserializer
        "../../../../source/m2mtlvserializer.cpp"
        "main.cpp"
        "m2mtlvserializertest.cpp"
        "test_m2mtlvserializer.cpp"
        "../stub/m2mstring_stub.cpp"
        "../stub/m2mbase_stub.cpp"
        "../stub/m2mresource_stub.cpp"
        "../stub/m2mresourceinstance_stub.cpp"
        "../stub/m2mobjectinstance_stub.cpp"
        "../stub/m2mobject_stub.cpp"
)

target_link_libraries(m2mtlvserializer
    mbed-client
    CppUTest
    CppUTestExt
)

add_test(m2mtlvserializer m2mtlvserializer)


endif()
