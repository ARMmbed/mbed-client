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
	"../stub/m2mtlvdeserializer_stub.cpp"
)

target_link_libraries(m2mtlvserializer
    CppUTest
    CppUTestExt
)
set_target_properties(m2mtlvserializer
PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS}"
           LINK_FLAGS "${UTEST_LINK_FLAGS}")

set(binary "m2mtlvserializer -ojunit")
add_test(m2mtlvserializer ${binary})

endif()
