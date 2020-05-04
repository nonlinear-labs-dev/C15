cmake_minimum_required(VERSION 3.2)
project(C15-projects)

OPTION(BUILD_AUDIOENGINE "Enable building of the audio-engine sub project" On)
OPTION(BUILD_BBBB "Enable building of the BeagleBoneBlack-Bridge sub project" On)
OPTION(BUILD_PLAYGROUND "Enable building of the Playground and NonMaps sub projects" On)
OPTION(BUILD_ONLINEHELP "Enable building and installing of the C15 online help" Off)
OPTION(BUILD_TEXT2SOLED "Enable building and installing of the text2soled utility" Off)
OPTION(BUILD_TESTING "Enable tests" On)
OPTION(BUILD_REALTIME_SYSTEM "Enable building of the LPC realtime system" Off)

EXEC_PROGRAM("git" ${CMAKE_CURRENT_SOURCE_DIR} ARGS "log -n 1 --pretty='format:%cd' --date=format:'%Y-%m-%d %H:%M:%S'" OUTPUT_VARIABLE LAST_COMMIT_STAMP)
EXEC_PROGRAM("git" ${CMAKE_CURRENT_SOURCE_DIR} ARGS "rev-parse --abbrev-ref HEAD" OUTPUT_VARIABLE BRANCH_NAME)
SET(C15_VERSION "${LAST_COMMIT_STAMP}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wdouble-promotion -Wno-deprecated-declarations")

find_package(PkgConfig REQUIRED)
pkg_check_modules(SYSTEMD "systemd")

if (SYSTEMD_FOUND AND !DEV_PC)
    execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=systemdsystemunitdir systemd OUTPUT_VARIABLE SYSTEMD_SERVICES_INSTALL_DIR)
    string(REGEX REPLACE "[ \t\n]+" "" SYSTEMD_SERVICES_INSTALL_DIR "${SYSTEMD_SERVICES_INSTALL_DIR}")
else ()
    set(SYSTEMD_SERVICES_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib/systemd/system)
endif ()

message("SystemD service files will be installed to ${SYSTEMD_SERVICES_INSTALL_DIR}")

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    message("This build will use CCache.")
endif()

message("building C15 firmware version: ${C15_VERSION}")

SET(C15_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/C15)

message("Installing into: ${C15_INSTALL_PATH}")

add_subdirectory(third-party)

IF(BUILD_AUDIOENGINE OR BUILD_PLAYGROUND OR BUILD_BBBB)
    add_subdirectory(nltools)
    set(CMAKE_INSTALL_RPATH ${C15_INSTALL_PATH}/nltools)
ENDIF()

IF(BUILD_AUDIOENGINE OR BUILD_PLAYGROUND)
    add_subdirectory(parameter-db)
ENDIF()

IF(BUILD_AUDIOENGINE)
    add_subdirectory(audio-engine)
ENDIF(BUILD_AUDIOENGINE)

IF(BUILD_PLAYGROUND)
    add_subdirectory(playground)
    add_subdirectory(NonMaps)
ENDIF(BUILD_PLAYGROUND)

IF(BUILD_BBBB)
    add_subdirectory(bbbb)
ENDIF(BUILD_BBBB)

IF(BUILD_ONLINEHELP)
    add_subdirectory(online-help)
ENDIF(BUILD_ONLINEHELP)

IF(BUILD_TEXT2SOLED)
    add_subdirectory(text2soled)
ENDIF(BUILD_TEXT2SOLED)

IF(BUILD_REALTIME_SYSTEM)
    add_subdirectory(realtime-system)
ENDIF(BUILD_REALTIME_SYSTEM)

IF(BUILD_PLAYGROUND AND BUILD_AUDIOENGINE)
    add_subdirectory(acceptance-tests)
ENDIF()

add_subdirectory(scripts)
