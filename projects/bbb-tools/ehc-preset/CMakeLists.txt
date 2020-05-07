cmake_minimum_required(VERSION 3.0)

project(ehc-preset)

file(GLOB_RECURSE SOURCE_FILES ./*.c ${CMAKE_CURRENT_SOURCE_DIR}/../../shared/)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)


