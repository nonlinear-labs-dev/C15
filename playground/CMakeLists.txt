cmake_minimum_required(VERSION 3.5)
project(playground)
project(playground-tests)

execute_process(COMMAND "git" "rev-parse" "--abbrev-ref" "HEAD"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  RESULT_VARIABLE git_result
  OUTPUT_VARIABLE BRANCH_NAME)

execute_process(COMMAND "git" "rev-list" "HEAD" "--count"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  RESULT_VARIABLE git_result
  OUTPUT_VARIABLE COMMIT_COUNT)

execute_process(COMMAND "git" "rev-parse" "--short" "HEAD"
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  RESULT_VARIABLE git_result
  OUTPUT_VARIABLE HEAD_REF)

string(STRIP ${COMMIT_COUNT} COMMIT_COUNT)
string(STRIP ${HEAD_REF} HEAD_REF)
string(STRIP ${BRANCH_NAME} BRANCH_NAME)

set(LIBS
  glibmm-2.4
  gio-unix-2.0
  giomm-2.4
  uuid
  libsoup-2.4
  freetype2
  avahi-glib
  avahi-gobject
  avahi-core
  avahi-client
)

OPTION(DEV_PC "Enable some tests and tools not suitable for embedded device" OFF)
OPTION(PROFILING "Enable built in profiler" OFF)
OPTION(DSL_LAYOUTS "Enable experimental and not yet working DSL for HWUI layouts" OFF)

IF(NOT DEFINED SYSTEMD_CONFIGURATION_FILES_DIR)
  set(SYSTEMD_CONFIGURATION_FILES_DIR "${CMAKE_INSTALL_PREFIX}/etc/systemd/system")
ENDIF()

IF(DEV_PC)
  ADD_DEFINITIONS(-D_DEVELOPMENT_PC -D_TESTS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wmaybe-uninitialized -Wuninitialized -Wdouble-promotion -Wreturn-type")
  add_subdirectory(src/third-party)
ENDIF(DEV_PC)

IF(PROFILING)
  ADD_DEFINITIONS(-D_PROFILING)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finstrument-functions -finstrument-functions-exclude-file-list=Profiler.h,Profiler.cpp,map,/c++/")
ENDIF(PROFILING)

IF(DSL_LAYOUTS)
  ADD_DEFINITIONS(-D_DSL_LAYOUTS)
ENDIF(DSL_LAYOUTS)

function(addLib name)
  pkg_check_modules(${name} REQUIRED ${name})
  include_directories(${${name}_INCLUDE_DIRS})
  link_directories(${${name}_LIBRARY_DIRS})
endfunction(addLib)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(Freetype REQUIRED)

foreach(lib ${LIBS})
  addLib(${lib})
endforeach(lib)

include_directories(src)
include_directories(${PROJECT_BINARY_DIR}/src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wdouble-promotion -Wno-deprecated-declarations")

configure_file(src/CompileTimeOptions.h.in src/CompileTimeOptions.h)
configure_file(src/device-info/SoftwareVersion.cpp.in src/device-info/SoftwareVersion.cpp)
configure_file(src/proxies/hwui/panel-unit/boled/setup/UISoftwareVersionEditor.cpp.in src/proxies/hwui/panel-unit/boled/setup/UISoftwareVersionEditor.cpp)

SET(GENERATED_SOURCES
  ${PROJECT_BINARY_DIR}/src/device-info/SoftwareVersion.cpp
  ${PROJECT_BINARY_DIR}/src/proxies/hwui/panel-unit/boled/setup/UISoftwareVersionEditor.cpp)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
file(GLOB_RECURSE DSL_SOURCE_FILES src/proxies/hwui/descriptive-layouts/dsl/*.cpp)

IF(NOT DSL_LAYOUTS)
  list(REMOVE_ITEM SOURCE_FILES EXCLUDE REGEX ${DSL_SOURCE_FILES})
ENDIF()

list(REMOVE_ITEM SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/playground.cpp)
list(REMOVE_ITEM SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/testing/unit-tests/playground-tests.cpp)

file(GLOB_RECURSE UNIT_TEST_FILES src/testing/unit-tests/*.cpp)
list(REMOVE_ITEM SOURCE_FILES EXCLUDE REGEX ${UNIT_TEST_FILES})

add_library(playground-lib STATIC ${SOURCE_FILES} ${GENERATED_SOURCES})

target_link_libraries(playground-lib pthread)
target_link_libraries(playground-lib stdc++fs)
target_link_libraries(playground-lib nltools)
target_link_libraries(playground-lib parameterDB)

include_directories(playground-lib ${CMAKE_SOURCE_DIR}/nltools/include/)
include_directories(playground-lib ${CMAKE_SOURCE_DIR}/playground/src)

function(linkLib name)
  pkg_check_modules(${name} REQUIRED ${name})
  target_link_libraries(playground-lib ${${name}_LIBRARIES})
endfunction(linkLib)

foreach(lib ${LIBS})
  linkLib(${lib})
endforeach(lib)

set(PLAYGROUND_SOURCES
  "src/playground.cpp"
  )

set(PLAYGROUND_TEST_SOURCES
  ${UNIT_TEST_FILES})

add_executable(playground ${PLAYGROUND_SOURCES})
add_executable(playground-test ${PLAYGROUND_TEST_SOURCES})

target_link_libraries(playground PRIVATE playground-lib)
target_link_libraries(playground-test PRIVATE playground-lib)


configure_file(${PROJECT_SOURCE_DIR}/systemd/playground.service.in ${PROJECT_BINARY_DIR}/systemd/playground.service @ONLY)

file(GLOB PARAMETER_DESCRIPTIONS src/parameters/descriptions/*.txt)
file(GLOB VERSION_INFOS resources/version-infos/*)

add_custom_command(TARGET playground POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/../nltools/libnltools.so ${CMAKE_INSTALL_PREFIX}/nonlinear/playground/libnltools.so
)

add_custom_command(TARGET playground POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_BINARY_DIR}/../nltools/libparameterDB.so ${CMAKE_INSTALL_PREFIX}/nonlinear/playground/libparameterDB.so
)

install(FILES ${PROJECT_BINARY_DIR}/systemd/playground.service
  DESTINATION ${SYSTEMD_CONFIGURATION_FILES_DIR} COMPONENT init
)

install(FILES ${PARAMETER_DESCRIPTIONS}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/nonlinear/playground/resources/parameter-descriptions
)

install(FILES ${VERSION_INFOS}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/nonlinear/playground/resources/version-infos
)

install(DIRECTORY resources
  USE_SOURCE_PERMISSIONS
  DESTINATION ${CMAKE_INSTALL_PREFIX}/nonlinear/playground
)

install(TARGETS playground
  DESTINATION ${CMAKE_INSTALL_PREFIX}/nonlinear/playground
)

install(TARGETS playground-test
  DESTINATION ${CMAKE_INSTALL_PREFIX}/nonlinear/playground
)

ADD_CUSTOM_TARGET(
  touch-cmakelists ALL
  COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_LIST_FILE}
)


