FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG main
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(TEST_SOURCE_FILES
        GraphTest.cpp
        TramTest.cpp
        StationTest.cpp
)

add_executable(unit_tests ${TEST_SOURCE_FILES})

target_include_directories(unit_tests PUBLIC autobusLib)
target_link_libraries(unit_tests PUBLIC autobusLib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests)