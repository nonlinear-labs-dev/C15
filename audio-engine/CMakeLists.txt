cmake_minimum_required(VERSION 3.0)
project(audio-engine)

set(LIBS alsa)

EXEC_PROGRAM("sh"
  ${CMAKE_CURRENT_SOURCE_DIR}
  ARGS "-c \"git branch | grep '^\\* ' | sed 's/^\\* //'\" "
  OUTPUT_VARIABLE GIT_BRANCH)

SET(EXECUTABLE_NAME_WITH_BRANCH "audio-engine-${GIT_BRANCH}")

IF(CLANG)
    SET (CMAKE_C_COMPILER "/usr/bin/clang")
    SET (CMAKE_CXX_COMPILER "/usr/bin/clang++")
ENDIF(CLANG)

find_package(OpenMP)

IF(OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
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

include_directories(src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wdouble-promotion -march=native")

IF(GPO_COLLECT)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -fprofile-dir=/tmp/data/pgo -fprofile-generate=/tmp/data/pgo")
ENDIF(GPO_COLLECT)

IF(GPO_APPLY)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -fprofile-dir=/tmp/data/pgo -fprofile-use=/tmp/data/pgo -fprofile-correction")
ENDIF(GPO_APPLY)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_executable(audio-engine ${SOURCE_FILES})

IF(DEV_PC)
    TARGET_COMPILE_DEFINITIONS(audio-engine PRIVATE test_inputModeFlag=1)
ENDIF(DEV_PC)

target_link_libraries(audio-engine pthread)
target_link_libraries(audio-engine nltools)
include_directories(audio-engine ${CMAKE_SOURCE_DIR}/nltools/include/)

function(linkLib name)
  pkg_check_modules(${name} REQUIRED ${name})
  target_link_libraries(audio-engine ${${name}_LIBRARIES})
endfunction(linkLib)

foreach(lib ${LIBS})
  linkLib(${lib})
endforeach(lib)

add_custom_command(TARGET audio-engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:audio-engine> ${EXECUTABLE_NAME_WITH_BRANCH}
)

install(TARGETS audio-engine
  DESTINATION ${CMAKE_INSTALL_PREFIX}/nonlinear/audio-engine
  )

ADD_CUSTOM_TARGET(
	audio-engine-touch-cmakelists ALL
	COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_LIST_FILE}
)

if(NOT DEFINED SYSTEMD_CONFIGURATION_FILES_DIR)
       set(SYSTEMD_CONFIGURATION_FILES_DIR "${CMAKE_INSTALL_PREFIX}/etc/systemd/system")
ENDIF()

configure_file(${PROJECT_SOURCE_DIR}/systemd/audio-engine.service.in ${PROJECT_BINARY_DIR}/systemd/audio-engine.service @ONLY)
install(FILES ${PROJECT_BINARY_DIR}/systemd/audio-engine.service DESTINATION ${SYSTEMD_CONFIGURATION_FILES_DIR} COMPONENT init)
