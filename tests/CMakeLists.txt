FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)


set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(TEST_SOURCE_FILES
        GraphTest.cpp
        TramTest.cpp
        StationTest.cpp
        GuiUtilityTest.cpp
)

add_executable(unit_tests ${TEST_SOURCE_FILES})

target_include_directories(unit_tests PUBLIC autobusLib)
target_link_libraries(unit_tests PUBLIC autobusLib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests)