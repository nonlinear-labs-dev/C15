cmake_minimum_required(VERSION 3.0)
project(audio-engine-tests)

file(GLOB_RECURSE TEST_FILES *.cpp)

add_executable(${PROJECT_NAME} ${TEST_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${AE_LIB_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE ../src)
