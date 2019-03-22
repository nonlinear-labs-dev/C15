cmake_minimum_required(VERSION 3.0)
project(audio-engine)

set(LIBS
  glibmm-2.4
  giomm-2.4
  glib-2.0
  gio-2.0
  alsa
  )

IF(CLANG)
    message ("Using clang compiler")
    SET (CMAKE_C_COMPILER "/usr/bin/clang")
    SET (CMAKE_CXX_COMPILER "/usr/bin/clang++")
ELSEIF(ICC)
    message ("Using intel compiler")
    SET (CMAKE_C_COMPILER "/opt/intel/bin/icc")
    SET (CMAKE_CXX_COMPILER "/opt/intel/bin/icc")
ENDIF(CLANG)

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

include_directories(src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wdouble-promotion")

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_executable(audio-engine ${SOURCE_FILES})

IF(DEV_PC)
    TARGET_COMPILE_DEFINITIONS(audio-engine PRIVATE test_inputModeFlag=1)
ENDIF(DEV_PC)

TARGET_LINK_LIBRARIES(audio-engine pthread)

function(linkLib name)
  pkg_check_modules(${name} REQUIRED ${name})
  target_link_libraries(audio-engine ${${name}_LIBRARIES})
endfunction(linkLib)

foreach(lib ${LIBS})
  linkLib(${lib})
endforeach(lib)

install(TARGETS audio-engine
  DESTINATION ${CMAKE_INSTALL_PREFIX}/nonlinear/audio-engine
  )

ADD_CUSTOM_TARGET(
	audio-engine-touch-cmakelists ALL
	COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_LIST_FILE}
)




