if(TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(m2mdevice
        "main.cpp"
        "../stub/m2mobject_stub.cpp"
        "../stub/m2mobjectinstance_stub.cpp"
        "../stub/m2mresource_stub.cpp"
        "../stub/m2mresourceinstance_stub.cpp"
        "../stub/m2mbase_stub.cpp"
        "../stub/m2mstring_stub.cpp"
        "m2mdevicetest.cpp"
        "test_m2mdevice.cpp"
        "../../../../source/m2mdevice.cpp"
        "../../../../source/m2mconstants.cpp"
)
target_link_libraries(m2mdevice
    CppUTest
    CppUTestExt
)
set_target_properties(m2mdevice
PROPERTIES COMPILE_FLAGS "${UTEST_COMPILE_FLAGS}"
           LINK_FLAGS "${UTEST_LINK_FLAGS}")
add_test(m2mdevice m2mdevice)

endif()
