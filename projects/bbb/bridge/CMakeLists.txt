cmake_minimum_required(VERSION 3.0)
project(bbbb)

enable_ccache()
find_systemd()
prepare_nltools()

set(LIBS giomm-2.4 libsoup-2.4)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
file(GLOB_RECURSE SERVICES systemd/*.service.in)

IF(NOT DEV_PC)
    file(GLOB_RECURSE UI_SOURCES src/ui/*.cpp)
    list(REMOVE_ITEM SOURCE_FILES EXCLUDE REGEX ${UI_SOURCES})
ENDIF(NOT DEV_PC)

IF(CONTROLS)
    ADD_DEFINITIONS(-D_WINDOW_CONTROLS)
ENDIF(CONTROLS)

IF(DEV_PC)
  ADD_DEFINITIONS(-D_DEVELOPMENT_PC -D_TESTS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmaybe-uninitialized -Wuninitialized -Wreturn-type")
  set(LIBS ${LIBS} gtkmm-3.0 gdkmm-3.0 libpng)
ENDIF(DEV_PC)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_nltools(${PROJECT_NAME})

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wdouble-promotion")

foreach(lib ${LIBS})
  pkg_check_modules(${lib} REQUIRED ${lib})
  target_include_directories(${PROJECT_NAME} PUBLIC ${${lib}_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} PUBLIC ${${lib}_LIBRARIES})
  link_directories(${${lib}_LIBRARY_DIRS})
endforeach(lib)

target_link_libraries(${PROJECT_NAME} PUBLIC pthread)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

foreach(SERVICE ${SERVICES})
  string(REGEX REPLACE ".in$" "" WITHOUT_IN ${SERVICE})
  get_filename_component(SERVICENAME ${WITHOUT_IN} NAME)
  configure_file(${SERVICE} ${PROJECT_BINARY_DIR}/systemd/${SERVICENAME} @ONLY)
  install(FILES ${PROJECT_BINARY_DIR}/systemd/${SERVICENAME} DESTINATION ${SYSTEMD_SERVICES_INSTALL_DIR} COMPONENT init)

  string(REGEX MATCH ".*-oneshot.service$" IS_ONESHOT ${SERVICENAME})

  if("${IS_ONESHOT}" STREQUAL "")
      install(CODE "execute_process(
          COMMAND bash -c \"set -e
          mkdir -p $DESTDIR/${SYSTEMD_SERVICES_INSTALL_DIR}/multi-user.target.wants
          mkdir -p $DESTDIR/${SYSTEMD_SERVICES_INSTALL_DIR}/sysinit.target.wants
          ln -sf ../${SERVICENAME} $DESTDIR/${SYSTEMD_SERVICES_INSTALL_DIR}/multi-user.target.wants/${SERVICENAME}
          \")")
  endif()
endforeach(SERVICE)

install(DIRECTORY DESTINATION ${SYSTEMD_SERVICES_INSTALL_DIR}/multi-user.target.wants)
install(DIRECTORY DESTINATION ${SYSTEMD_SERVICES_INSTALL_DIR}/sysinit.target.wants)
install(TARGETS ${PROJECT_NAME} DESTINATION ${C15_INSTALL_PATH}/${PROJECT_NAME})
