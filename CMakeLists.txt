cmake_minimum_required(VERSION 3.10)
project(7laba)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")

add_library(npc_lib
    src/npc.cpp
    src/dragon.cpp
    src/bull.cpp
    src/toad.cpp
)
target_include_directories(npc_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_library(patterns_lib 
    src/factory.cpp
    src/observer.cpp
    src/visitor.cpp
)
target_include_directories(patterns_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(patterns_lib npc_lib)

add_executable(main src/main.cpp)
target_link_libraries(main patterns_lib npc_lib pthread)

if(NOT TARGET gtest)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)
endif()

enable_testing()
add_executable(tests tests/testcases.cpp)
target_link_libraries(tests gtest_main patterns_lib npc_lib pthread)

add_test(NAME DungeonTests COMMAND tests)