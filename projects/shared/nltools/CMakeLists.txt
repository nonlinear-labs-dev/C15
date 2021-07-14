cmake_minimum_required(VERSION 3.2)

project(nltools)

add_subdirectory(tests)

set(BUILD_SHARED_LIBS ON)

file(GLOB_RECURSE HEADER_FILES ./include/*.h)
file(GLOB_RECURSE SOURCE_FILES ./*.cpp)
file(GLOB_RECURSE TEST_FILES ./tests/*)

list(REMOVE_ITEM SOURCE_FILES EXCLUDE REGEX ${TEST_FILES})

set(LIBS glibmm-2.4 giomm-2.4 libsoup-2.4)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
find_package(PkgConfig REQUIRED)

foreach(lib ${LIBS})
  pkg_check_modules(${lib} REQUIRED ${lib})
  target_include_directories(${PROJECT_NAME} PUBLIC ${${lib}_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} PUBLIC ${${lib}_LIBRARIES})
  link_directories(${${lib}_LIBRARY_DIRS})
endforeach(lib)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wdouble-promotion")
install(TARGETS ${PROJECT_NAME} DESTINATION ${C15_INSTALL_PATH}/${PROJECT_NAME})

