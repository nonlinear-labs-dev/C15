cmake_minimum_required(VERSION 3.0)
project(audio-engine)

set(LIBS
  glibmm-2.4
  giomm-2.4
  glib-2.0
  gio-2.0
  alsa
  )

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

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wdouble-promotion -march=native")

IF(GPO_COLLECT)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -fprofile-dir=/tmp/data/pgo -fprofile-generate=/tmp/data/pgo")
ENDIF(GPO_COLLECT)

IF(GPO_APPLY)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -fprofile-dir=/tmp/data/pgo -fprofile-use=/tmp/data/pgo -fprofile-correction")
ENDIF(GPO_APPLY)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

# generated source files
file(GLOB_RECURSE SOURCE_FILE_GENERATORS ${CMAKE_CURRENT_SOURCE_DIR}/src/synth/c15-audio-engine/code-generators/header-generators/*.sh)

function(processHeaderGenerator generatorScript)
    string(REPLACE .sh .h generatedHeader ${generatorScript})
    string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} generatedHeader ${generatedHeader})
    set(GENERATED_SOURCE_FILES ${GENERATED_SOURCE_FILES} ${generatedHeader} PARENT_SCOPE)
    get_filename_component(PARENT_DIR ${generatedHeader} DIRECTORY)

    add_custom_command(OUTPUT ${generatedHeader}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PARENT_DIR}
        COMMAND sh ${generatorScript} > ${generatedHeader}
        DEPENDS ${generatorScript}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/synth/c15-audio-engine/pe_defines_lists.h
    )
endfunction(processHeaderGenerator)

foreach(line IN LISTS SOURCE_FILE_GENERATORS)
    processHeaderGenerator(${line})
endforeach()

add_executable(audio-engine ${GENERATED_SOURCE_FILES} ${SOURCE_FILES})

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







