cmake_minimum_required(VERSION 3.0)
project(nltools-tests)

prepare_nltools()

file(GLOB_RECURSE TEST_FILES *.cpp)
add_executable(${PROJECT_NAME} ${TEST_FILES})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/projects/shared/third-party/include)
add_nltools(${PROJECT_NAME})
