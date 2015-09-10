if(TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(m2mtlvdeserializer
        "../../../../source/m2mtlvdeserializer.cpp"
        "main.cpp"
        "m2mtlvdeserializertest.cpp"
        "test_m2mtlvdeserializer.cpp"
        "../stub/m2mstring_stub.cpp"
        "../stub/m2mbase_stub.cpp"
        "../stub/m2mresource_stub.cpp"
        "../stub/m2mresourceinstance_stub.cpp"
        "../stub/m2mobjectinstance_stub.cpp"
        "../stub/m2mobject_stub.cpp"
)

target_link_libraries(m2mtlvdeserializer
    mbed-client
    CppUTest
    CppUTestExt
)

add_test(m2mtlvdeserializer m2mtlvdeserializer)

endif()
