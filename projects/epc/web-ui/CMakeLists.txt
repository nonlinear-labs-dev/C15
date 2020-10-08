cmake_minimum_required(VERSION 3.0)
project(NonMaps)

add_subdirectory(recorder)

set(NONMAPS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(OBJDIR ${CMAKE_CURRENT_BINARY_DIR}/obj)
set(OUTFILE ${CMAKE_CURRENT_BINARY_DIR}/war/nonmaps/compilation-mappings.txt)
set(GWT_VERSION 2.9.0)

file(GLOB_RECURSE SOURCE_FILES ${NONMAPS_ROOT}/src/*.java ${NONMAPS_ROOT}/src/*.xml)

add_custom_command(
  COMMENT "Download gwt"
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gwt-${GWT_VERSION}.zip
  COMMAND wget https://storage.googleapis.com/gwt-releases/gwt-${GWT_VERSION}.zip -O ${CMAKE_CURRENT_BINARY_DIR}/gwt-${GWT_VERSION}.zip)

add_custom_command(
  COMMENT "Unpack gwt"
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gwt-${GWT_VERSION}/gwt-dev.jar
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gwt-${GWT_VERSION}.zip
  COMMAND unzip -uq ${CMAKE_CURRENT_BINARY_DIR}/gwt-${GWT_VERSION}.zip)

add_custom_command(
  OUTPUT ${OUTFILE}
  COMMAND /usr/bin/java -Xmx512m -classpath ${NONMAPS_ROOT}/src/main/java:${CMAKE_CURRENT_BINARY_DIR}/gwt-${GWT_VERSION}/gwt-user.jar:${CMAKE_CURRENT_BINARY_DIR}/gwt-${GWT_VERSION}/gwt-dev.jar com.google.gwt.dev.Compiler -sourceLevel 11 -optimize 9 -gen ${OBJDIR} -logLevel WARN -style OBF -war ./war -localWorkers 4 com.nonlinearlabs.NonMaps
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gwt-${GWT_VERSION}/gwt-dev.jar
#  DEPENDS ${SOURCE_FILES}
  )

add_custom_target(NonMaps ALL SOURCES ${OUTFILE})

file(WRITE ${OBJDIR}/nonmaps-version.txt ${C15_VERSION})

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/war DESTINATION ${C15_INSTALL_PATH}/NonMaps)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/war/nonmaps DESTINATION ${C15_INSTALL_PATH}/NonMaps/war)
install(DIRECTORY ${NONMAPS_ROOT}/src/main/webapp/ DESTINATION ${C15_INSTALL_PATH}/NonMaps/war)
install(DIRECTORY MCView DESTINATION ${C15_INSTALL_PATH}/NonMaps)
install(FILES ${OBJDIR}/nonmaps-version.txt DESTINATION ${C15_INSTALL_PATH}/NonMaps/war)
