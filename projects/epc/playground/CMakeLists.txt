cmake_minimum_required(VERSION 3.5)
project(playground)

find_systemd()
enable_ccache()
prepare_nltools()

# PRECONDITIONS
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(Freetype REQUIRED)

# VARS
SET(BINARY_NAME ${PROJECT_NAME})
SET(LIB_NAME ${PROJECT_NAME}-lib)
SET(TEST_BINARY_NAME ${PROJECT_NAME}-test)

execute_process(COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    RESULT_VARIABLE git_result
    OUTPUT_VARIABLE BRANCH_NAME)

execute_process(COMMAND git rev-list HEAD --count
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    RESULT_VARIABLE git_result
    OUTPUT_VARIABLE COMMIT_COUNT)

execute_process(COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    RESULT_VARIABLE git_result
    OUTPUT_VARIABLE HEAD_REF)
    
execute_process(COMMAND git log -n 1 "--pretty=format:%cd" "--date=format:%Y-%m-%d %H:%M:%S"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    RESULT_VARIABLE git_result
    OUTPUT_VARIABLE COMMIT_DATE)


string(STRIP ${COMMIT_COUNT} COMMIT_COUNT)
string(STRIP ${HEAD_REF} HEAD_REF)
string(STRIP ${BRANCH_NAME} BRANCH_NAME)
string(STRIP ${COMMIT_DATE} COMMIT_DATE)

set(LIBS gio-unix-2.0 giomm-2.4 uuid libsoup-2.4 freetype2 libpng)

# OPTIONS
OPTION(PROFILING "Enable built in profiler" OFF)
OPTION(DSL_LAYOUTS "Enable experimental and not yet working DSL for HWUI layouts" OFF)

IF(DEV_PC)
  ADD_DEFINITIONS(-D_DEVELOPMENT_PC -D_TESTS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wmaybe-uninitialized -Wuninitialized -Wdouble-promotion -Wreturn-type")
ENDIF(DEV_PC)

IF(PROFILING)
  ADD_DEFINITIONS(-D_PROFILING)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finstrument-functions -finstrument-functions-exclude-file-list=Profiler.h,Profiler.cpp,map,/c++/")
ENDIF(PROFILING)

IF(DSL_LAYOUTS)
  ADD_DEFINITIONS(-D_DSL_LAYOUTS)
ENDIF(DSL_LAYOUTS)

# CONFIGURED FILES
configure_file(src/device-info/BuildVersion.cpp.in src/device-info/BuildVersion.cpp)
configure_file(src/CompileTimeOptions.h.in src/CompileTimeOptions.h)
configure_file(src/proxies/hwui/panel-unit/boled/setup/UISoftwareVersionEditor.cpp.in src/proxies/hwui/panel-unit/boled/setup/UISoftwareVersionEditor.cpp)
configure_file(${PROJECT_SOURCE_DIR}/systemd/${BINARY_NAME}.service.in ${PROJECT_BINARY_DIR}/systemd/${BINARY_NAME}.service @ONLY)

# SOURCE FILE COLLECTIONS
SET(GENERATED_SOURCES ${PROJECT_BINARY_DIR}/src/proxies/hwui/panel-unit/boled/setup/UISoftwareVersionEditor.cpp ${PROJECT_BINARY_DIR}/src/device-info/BuildVersion.cpp)
file(GLOB PARAMETER_DESCRIPTIONS src/parameters/descriptions/*.txt)
file(GLOB VERSION_INFOS resources/version-infos/*)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp ${CMAKE_SOURCE_DIR}/projects/shared/playcontroller/*.c)
file(GLOB_RECURSE DSL_SOURCE_FILES src/proxies/hwui/descriptive-layouts/dsl/*.cpp)

IF(NOT DSL_LAYOUTS)
    list(REMOVE_ITEM SOURCE_FILES EXCLUDE REGEX ${DSL_SOURCE_FILES})
ENDIF()

list(REMOVE_ITEM SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/playground.cpp)
list(REMOVE_ITEM SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/testing/unit-tests/playground-tests.cpp)
file(GLOB_RECURSE UNIT_TEST_FILES src/testing/*.cpp)
list(REMOVE_ITEM SOURCE_FILES EXCLUDE REGEX ${UNIT_TEST_FILES})
set(${BINARY_NAME}_SOURCES "src/playground.cpp")
set(${TEST_BINARY_NAME}_SOURCES ${UNIT_TEST_FILES})

# TARGETS
## LIB
add_library(${LIB_NAME} STATIC ${SOURCE_FILES} ${GENERATED_SOURCES})
add_nltools(${LIB_NAME})

foreach(lib ${LIBS})
  pkg_check_modules(${lib} REQUIRED ${lib})
  target_include_directories(${LIB_NAME} PUBLIC ${${lib}_INCLUDE_DIRS})
  target_link_libraries(${LIB_NAME} PUBLIC ${${lib}_LIBRARIES})
  link_directories(${${lib}_LIBRARY_DIRS})
endforeach(lib)

target_include_directories(${LIB_NAME} PUBLIC src)
target_include_directories(${LIB_NAME} PUBLIC ${PROJECT_BINARY_DIR}/src)
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/projects/shared/parameter-db/generated/)
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/projects/epc/playground/src)
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/projects/shared)
target_link_libraries(${LIB_NAME} PUBLIC pthread stdc++fs)

## BINARY
add_executable(${BINARY_NAME} ${${BINARY_NAME}_SOURCES})
target_link_libraries(${BINARY_NAME} PRIVATE ${LIB_NAME})

## TEST BINARY
add_executable(${TEST_BINARY_NAME} ${${TEST_BINARY_NAME}_SOURCES})
target_include_directories(${TEST_BINARY_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/projects/shared/third-party/include)
target_link_libraries(${TEST_BINARY_NAME} PRIVATE ${LIB_NAME})

# TESTS
add_test(NAME ${PROJECT_NAME} COMMAND ${TEST_BINARY_NAME})

# INSTALLATION

install(FILES ${PARAMETER_DESCRIPTIONS} DESTINATION ${C15_INSTALL_PATH}/playground/resources/parameter-descriptions)
install(FILES ${VERSION_INFOS} DESTINATION ${C15_INSTALL_PATH}/playground/resources/version-infos)
install(DIRECTORY resources USE_SOURCE_PERMISSIONS DESTINATION ${C15_INSTALL_PATH}/playground)
install(TARGETS ${BINARY_NAME} DESTINATION ${C15_INSTALL_PATH}/${BINARY_NAME})

install(FILES ${PROJECT_BINARY_DIR}/systemd/${BINARY_NAME}.service DESTINATION ${SYSTEMD_SERVICES_INSTALL_DIR} COMPONENT init)
install(FILES ${PROJECT_SOURCE_DIR}/systemd/persistent.mount DESTINATION ${SYSTEMD_SERVICES_INSTALL_DIR} COMPONENT init)

install(DIRECTORY DESTINATION ${SYSTEMD_SERVICES_INSTALL_DIR}/multi-user.target.wants)
install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ../${BINARY_NAME}.service $ENV{DESTDIR}/${SYSTEMD_SERVICES_INSTALL_DIR}/multi-user.target.wants/${BINARY_NAME}.service)")
install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ../persistent.mount $ENV{DESTDIR}/${SYSTEMD_SERVICES_INSTALL_DIR}/multi-user.target.wants/persistent.mount)")
