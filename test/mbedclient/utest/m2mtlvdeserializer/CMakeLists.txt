if(TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(m2mtlvdeserializer
        "../../../../source/m2mtlvdeserializer.cpp"
        "../../../../source/m2mtlvserializer.cpp"
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
    CppUTest
    CppUTestExt
)
set_target_properties(m2mtlvdeserializer
PROPERTIES COMPILE_FLAGS "${UTEST_COMPILE_FLAGS}"
           LINK_FLAGS "${UTEST_LINK_FLAGS}")

set(binary "m2mtlvdeserializer -ojunit")
add_test(m2mtlvdeserializer ${binary})

endif()
