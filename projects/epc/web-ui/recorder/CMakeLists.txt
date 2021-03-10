cmake_minimum_required(VERSION 3.0)

project(Recorder)

file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*)

add_custom_command(
  COMMENT "Transpile TypeScript"
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/recorder.js
  DEPENDS ${SOURCE_FILES}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND tsc --outfile ${CMAKE_CURRENT_BINARY_DIR}/recorder.js
  )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/recorder.js DESTINATION ${C15_INSTALL_PATH}/NonMaps/recorder)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/index.html DESTINATION ${C15_INSTALL_PATH}/NonMaps/recorder)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/styles.css DESTINATION ${C15_INSTALL_PATH}/NonMaps/recorder)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/fonts DESTINATION ${C15_INSTALL_PATH}/NonMaps/recorder)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/img DESTINATION ${C15_INSTALL_PATH}/NonMaps/recorder)

add_custom_target(Recorder ALL SOURCES ${CMAKE_CURRENT_BINARY_DIR}/recorder.js)