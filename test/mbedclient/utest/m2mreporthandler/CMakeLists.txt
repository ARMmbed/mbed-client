if(TARGET_LIKE_LINUX)
include("../includes.txt")


add_executable(m2mreporthandler
        "main.cpp"
        "m2mreporthandlertest.cpp"
        "test_m2mreporthandler.cpp"
        "../stub/m2mtimer_stub.cpp"
        "../stub/m2mstring_stub.cpp"
        "../../../../source/m2mreporthandler.cpp"
        "../../../../source/m2mconstants.cpp"

)
target_link_libraries(m2mreporthandler
    CppUTest
    CppUTestExt
)

set_target_properties(m2mreporthandler
PROPERTIES COMPILE_FLAGS "${UTEST_COMPILE_FLAGS}"
           LINK_FLAGS "${UTEST_LINK_FLAGS}")
set(binary "m2mreporthandler -ojunit")
add_test(m2mreporthandler ${binary})

endif()
