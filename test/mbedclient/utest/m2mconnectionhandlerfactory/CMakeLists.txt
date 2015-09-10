if(TARGET_LIKE_LINUX)
include("../includes.txt")
add_executable(m2mconnectionhandlerfactory         
	"main.cpp"
	"m2mconnectionhandlerfactorytest.cpp"
       	"test_m2mconnectionhandlerfactory.cpp"
        "../stub/m2mconnectionhandler_stub.cpp"
        "../stub/m2mstring_stub.cpp"
        "../stub/m2mconnectionsecurity_stub.cpp"
        "../../../../source/m2mconnectionhandlerfactory.cpp"
)
target_link_libraries(m2mconnectionhandlerfactory
    CppUTest
    CppUTestExt
)

add_test(m2mconnectionhandlerfactory m2mconnectionhandlerfactory)

endif()
