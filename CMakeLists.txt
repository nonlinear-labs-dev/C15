cmake_minimum_required(VERSION 3.2)

project(C15)

include(CTest)
include(cmake/NL.cmake)

set(TARGET_PLATFORM "devpc" CACHE STRING "Which target to build for: devpc | epc | epc2 | bbb | playcontroller")

message("Target platform is: ${TARGET_PLATFORM}")
add_definitions(-DTARGET_PLATFORM="${TARGET_PLATFORM}")

add_subdirectory(projects)
add_subdirectory(build-tools)

