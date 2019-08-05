# Remove strings matching given regular expression from a list.
# @param(in,out) aItems Reference of a list variable to filter.
# @param aRegEx Value of regular expression to match.
function (filter_items aItems aRegEx)
    # For each item in our list
    foreach (item ${${aItems}})
        # Check if our items matches our regular expression
        if ("${item}" MATCHES ${aRegEx})
            # Remove current item from our list
            list (REMOVE_ITEM ${aItems} ${item})
        endif ("${item}" MATCHES ${aRegEx})
    endforeach(item)
    # Provide output parameter
    set(${aItems} ${${aItems}} PARENT_SCOPE)
endfunction (filter_items)


cmake_minimum_required(VERSION 3.0)
project(bbbb)

set(LIBS
  glibmm-2.4
  giomm-2.4
  glib-2.0
  gio-2.0
  gio-unix-2.0
  libsoup-2.4
)

IF(CONTROLS)
    ADD_DEFINITIONS(-D_WINDOW_CONTROLS)
ENDIF(CONTROLS)

IF(DEV_PC)
  ADD_DEFINITIONS(-D_DEVELOPMENT_PC -D_TESTS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wmaybe-uninitialized -Wuninitialized -Wdouble-promotion -Wreturn-type")
  set(LIBS ${LIBS} gtkmm-3.0 gdkmm-3.0 libpng)
ENDIF(DEV_PC)

IF(NOT DEFINED SYSTEMD_CONFIGURATION_FILES_DIR)
  set(SYSTEMD_CONFIGURATION_FILES_DIR "${CMAKE_INSTALL_PREFIX}/etc/systemd/system")
ENDIF()

function(addLib name)
  pkg_check_modules(${name} REQUIRED ${name})
  include_directories(${${name}_INCLUDE_DIRS})
  link_directories(${${name}_LIBRARY_DIRS})
endfunction(addLib)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

foreach(lib ${LIBS})
  addLib(${lib})
endforeach(lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wdouble-promotion")

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
    IF(DEV_PC)
        ELSE()
filter_items(SOURCE_FILES "/ui/")
    ENDIF(DEV_PC)


add_executable(bbbb ${SOURCE_FILES})

target_link_libraries(bbbb pthread)
target_link_libraries(bbbb nltools)
include_directories(audio-engine ${CMAKE_SOURCE_DIR}/nltools/include/)

function(linkLib name)
  pkg_check_modules(${name} REQUIRED ${name})
  target_link_libraries(bbbb ${${name}_LIBRARIES})
endfunction(linkLib)

foreach(lib ${LIBS})
  linkLib(${lib})
endforeach(lib)

configure_file(${PROJECT_SOURCE_DIR}/systemd/bbbb.service.in
	${PROJECT_BINARY_DIR}/systemd/bbbb.service @ONLY)

install(FILES ${PROJECT_BINARY_DIR}/systemd/bbbb.service
  DESTINATION ${SYSTEMD_CONFIGURATION_FILES_DIR} COMPONENT init
  )

install(TARGETS bbbb
  DESTINATION ${CMAKE_INSTALL_PREFIX}/nonlinear/playground
  )

ADD_CUSTOM_TARGET(
	touch-bbb-cmakelists ALL
	COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_LIST_FILE}
)

