cmake_minimum_required(VERSION 3.0)
project(playground)

set(LIBS
  glibmm-2.4
  giomm-2.4
  glib-2.0
  gio-2.0
  gio-unix-2.0
  libsoup-2.4
  uuid
  avahi-glib
  avahi-gobject
  avahi-core
  avahi-client
  freetype2 
  )

OPTION(DEV_PC
  "Enable some tests and tools not suitable for embedded device"
  OFF)

OPTION(PROFILING
  "Enable built in profiler"
  OFF)


IF(DEV_PC)
  ADD_DEFINITIONS(-D_DEVELOPMENT_PC -D_TESTS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wmaybe-uninitialized -Wuninitialized -Wdouble-promotion -Wreturn-type")
ENDIF(DEV_PC)

IF(PROFILING)
  ADD_DEFINITIONS(-D_PROFILING)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finstrument-functions -finstrument-functions-exclude-file-list=Profiler.h,Profiler.cpp,map,/c++/")
ENDIF(PROFILING)

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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wdouble-promotion")
  
ADD_CUSTOM_TARGET(
	invalidate_version_file ALL
	COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_SOURCE_DIR}/src/device-info/SoftwareVersion.cpp
)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp) 
  
add_executable(playground ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(playground pthread)
TARGET_LINK_LIBRARIES(playground stdc++fs)

function(linkLib name)
  pkg_check_modules(${name} REQUIRED ${name})
  target_link_libraries(playground ${${name}_LIBRARIES})
endfunction(linkLib)

foreach(lib ${LIBS})
  linkLib(${lib})
endforeach(lib)

set_property(
  SOURCE src/device-info/SoftwareVersion.cpp
  APPEND_STRING
  PROPERTY COMPILE_FLAGS
  "-DLAST_COMMIT_TIME=\"${PLAYGROUND_VERSION}\""
)

file(GLOB PARAMETER_DESCRIPTIONS src/parameters/descriptions/*.txt)

install(FILES ${PARAMETER_DESCRIPTIONS}
  DESTINATION resources/parameter-descriptions
  )

install(DIRECTORY resources
  USE_SOURCE_PERMISSIONS
  DESTINATION ./
  )

install(TARGETS playground
  DESTINATION ./
  )

ADD_CUSTOM_TARGET(
	touch-cmakelists ALL
	COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_LIST_FILE}
)


