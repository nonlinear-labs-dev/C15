OPTION(BUILD_ACCEPTANCE_TESTS "Build the acceptance tests for audio-engine and playground" Off)
OPTION(BUILD_AUDIOENGINE "Build the audio-engine" Off)
OPTION(BUILD_ONLINEHELP "Build the C15 html online help" Off)
OPTION(BUILD_PLAYGROUND "Build the playground" Off)
OPTION(BUILD_WEBUI "Build the NonMaps web-ui" Off)
OPTION(BUILD_EPC_SCRIPTS "Install the shell scripts on the ePC" Off)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wdouble-promotion -march=native")

IF(BUILD_ACCEPTANCE_TESTS)
  message ("FIX THIS!")
    #add_subdirectory(acceptance-tests)
ENDIF()

IF(BUILD_AUDIOENGINE)
    add_subdirectory(audio-engine)
ENDIF()

IF(BUILD_ONLINEHELP)
    add_subdirectory(online-help)
ENDIF()

IF(BUILD_PLAYGROUND)
    add_subdirectory(playground)
ENDIF()

IF(BUILD_WEBUI)
    add_subdirectory(web-ui)
ENDIF()

IF(BUILD_EPC_SCRIPTS)
    add_subdirectory(scripts)
ENDIF()

add_subdirectory(third-party)
