if(TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(m2mconnectionhandlerimpl_linux
        "../../../../yotta_modules/mbed-client-linux/source/m2mconnectionhandler.cpp"
        "../../../../yotta_modules/mbed-client-linux/source/connthreadhelper.cpp"
	"main.cpp"
        "m2mconnectionhandlerimpl_linuxtest.cpp"
        "test_m2mconnectionhandlerimpl_linux.cpp"
       "../stub/m2mconnectionhandlerpimpl_stub.cpp"
       "../stub/common_stub.cpp"
       "../stub/nsdlaccesshelper_stub.cpp"
       "../stub/m2mstring_stub.cpp"
       "../stub/m2mconnectionsecurity_stub.cpp"
)
target_link_libraries(m2mconnectionhandlerimpl_linux
   CppUTest
   CppUTestExt
)
set_target_properties(m2mconnectionhandlerimpl_linux
PROPERTIES COMPILE_FLAGS "${UTEST_COMPILE_FLAGS}"
           LINK_FLAGS "${UTEST_LINK_FLAGS}")
set(binary "m2mconnectionhandlerimpl_linux -ojunit")
add_test(m2mconnectionhandlerimpl_linux ${binary})

endif()
