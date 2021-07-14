cmake_minimum_required(VERSION 3.0)
project(acceptance-tests)

prepare_nltools()

find_package(PkgConfig REQUIRED)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_nltools(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE ${AE_LIB_NAME})

include_directories(src)

target_link_libraries(${PROJECT_NAME} PUBLIC pthread audio-engine-lib playground-lib)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/projects/shared/third-party/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/projects/epc/playground/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/projects/epc/audio-engine/src)
