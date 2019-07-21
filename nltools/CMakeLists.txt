cmake_minimum_required(VERSION 3.2)

project(nltools)

set(BUILD_SHARED_LIBS ON)

include(GNUInstallDirs)

file(GLOB_RECURSE HEADER_FILES ./include/*.h)
file(GLOB_RECURSE SOURCE_FILES ./*.cpp)

set(LIBS
  glibmm-2.4
  giomm-2.4
  libsoup-2.4
  )

add_library(nltools SHARED ${SOURCE_FILES})
find_package(PkgConfig REQUIRED)

function(addLib name)
  pkg_check_modules(${name} REQUIRED ${name})
  target_include_directories(nltools PUBLIC ${${name}_INCLUDE_DIRS})
  target_link_libraries(nltools PUBLIC ${${name}_LIBRARIES})
  link_directories(${${name}_LIBRARY_DIRS})
endfunction(addLib)

foreach(lib ${LIBS})
  addLib(${lib})
endforeach(lib)

target_include_directories(
    nltools INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>)

include_directories(nltools ${CMAKE_CURRENT_SOURCE_DIR}/../)
include_directories(nltools ${CMAKE_CURRENT_SOURCE_DIR}/include/)
include_directories(nltools ${CMAKE_CURRENT_SOURCE_DIR}/src/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wdouble-promotion")

set(headers_install_dir ${CMAKE_INSTALL_INCLUDEDIR}/nltools)

install(FILES ${HEADER_FILES} DESTINATION ${headers_install_dir})
install(TARGETS nltools DESTINATION ${CMAKE_INSTALL_LIBDIR})

