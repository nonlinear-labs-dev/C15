cmake_minimum_required(VERSION 3.2)

project(C15)

EXEC_PROGRAM("git"
  ${CMAKE_CURRENT_SOURCE_DIR}
  ARGS "log -1 --pretty=format:\"%ad\" --date=format:\"%Y-%m-%d %H:%M\""
  OUTPUT_VARIABLE PLAYGROUND_VERSION)

message("building C15 firmware version: ${PLAYGROUND_VERSION}")

OPTION(BUILD_AUDIOENGINE "Enable building of the audio-engine sub project" ON)
OPTION(BUILD_BBBB "Enable building of the BeagleBoneBlack-Bridge sub project" ON)
OPTION(BUILD_PLAYGROUND "Enable building of the Playground and NonMaps sub projects" ON)

add_subdirectory(nltools)

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
