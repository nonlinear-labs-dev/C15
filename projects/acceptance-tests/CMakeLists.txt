cmake_minimum_required(VERSION 3.0)
project(acceptance-tests)

find_package(PkgConfig REQUIRED)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${AE_LIB_NAME})

include_directories(src)

target_link_libraries(${PROJECT_NAME} PUBLIC pthread nltools audio-engine-lib playground-lib)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/projects/nltools/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/projects/third-party/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/projects/playground/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/projects/audio-engine/src)
