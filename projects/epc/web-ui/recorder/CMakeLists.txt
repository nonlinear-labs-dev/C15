cmake_minimum_required(VERSION 3.0)

project(Recorder)

file(GLOB_RECURSE TS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.ts)
file(GLOB_RECURSE ALL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/constants.ts.in ${CMAKE_CURRENT_BINARY_DIR}/ts/constants.ts)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tsconfig.json ${CMAKE_CURRENT_BINARY_DIR}/ts/tsconfig.json)  

add_custom_command(
    COMMENT "Transpile TypeScript"
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/recorder.js
    DEPENDS ${ALL_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND tsc --project ${CMAKE_CURRENT_BINARY_DIR}/ts/tsconfig.json)



install(FILES ${CMAKE_CURRENT_BINARY_DIR}/recorder.js DESTINATION ${C15_INSTALL_PATH}/NonMaps/recorder)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/index.html DESTINATION ${C15_INSTALL_PATH}/NonMaps/recorder)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/styles.css DESTINATION ${C15_INSTALL_PATH}/NonMaps/recorder)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/favicon.ico DESTINATION ${C15_INSTALL_PATH}/NonMaps/recorder)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/fonts DESTINATION ${C15_INSTALL_PATH}/NonMaps/recorder)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/img DESTINATION ${C15_INSTALL_PATH}/NonMaps/recorder)

add_custom_target(Recorder ALL SOURCES ${CMAKE_CURRENT_BINARY_DIR}/recorder.js)
