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
set_target_properties(m2mtlvdeserializer
PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage -fprofile-arcs -ftest-coverage -D__thumb2__ -w"
           LINK_FLAGS "-fprofile-arcs")
add_test(m2mtlvdeserializer m2mtlvdeserializer)

endif()
