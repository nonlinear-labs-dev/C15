cmake_minimum_required(VERSION 3.2)

OPTION(DEV_PC "Enable debugging features to be run on a development PC" Off)

EXEC_PROGRAM("git" ${CMAKE_CURRENT_SOURCE_DIR} ARGS "log -n 1 --pretty='format:%cd' --date=format:'%Y-%m-%d %H:%M:%S'" OUTPUT_VARIABLE LAST_COMMIT_STAMP)
EXEC_PROGRAM("git" ${CMAKE_CURRENT_SOURCE_DIR} ARGS "rev-parse --abbrev-ref HEAD" OUTPUT_VARIABLE BRANCH_NAME)
SET(C15_VERSION "${LAST_COMMIT_STAMP}")

message("building C15 firmware version: ${C15_VERSION}")

SET(C15_INSTALL_PATH ${CMAKE_INSTALL_PREFIX}/C15)

add_subdirectory(bbb)
add_subdirectory(epc)
add_subdirectory(playcontroller)
add_subdirectory(shared)
add_subdirectory(supervisor)
add_subdirectory(midibridge)
