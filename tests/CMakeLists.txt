if ( CONFIG_BUILD_TESTS )

find_package(Catch2 REQUIRED)

add_executable(tests tests.cpp reg_tests.cpp)
target_link_libraries(tests core)

endif ()
