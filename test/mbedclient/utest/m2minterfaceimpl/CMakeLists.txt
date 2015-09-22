if(TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(m2minterfaceimpl
        "main.cpp"
        "m2minterfaceimpltest.cpp"
        "test_m2minterfaceimpl.cpp"
        "../stub/m2mobject_stub.cpp"
        "../stub/m2mbase_stub.cpp"
        "../stub/m2mobject_stub.cpp"
        "../stub/m2mstring_stub.cpp"
        "../stub/m2mresource_stub.cpp"
        "../stub/m2mresourceinstance_stub.cpp"
        "../stub/m2mobjectinstance_stub.cpp"
        "../stub/m2msecurity_stub.cpp"
        "../stub/m2mtimer_stub.cpp"
        "../stub/m2mnsdlinterface_stub.cpp"
        "../stub/m2mconnectionhandler_stub.cpp"
        "../stub/m2mconnectionsecurity_stub.cpp"
        "../../../../source/m2minterfaceimpl.cpp"
        "../../../../source/m2mconstants.cpp"
)
target_link_libraries(m2minterfaceimpl
    CppUTest
    CppUTestExt
)
set_target_properties(m2minterfaceimpl
PROPERTIES COMPILE_FLAGS "${UTEST_COMPILE_FLAGS}"
           LINK_FLAGS "${UTEST_LINK_FLAGS}")
add_test(m2minterfaceimpl m2minterfaceimpl)

endif()
