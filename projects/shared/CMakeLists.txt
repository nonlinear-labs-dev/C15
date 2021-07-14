
IF(BUILD_PLAYGROUND OR BUILD_AUDIOENGINE OR BUILD_BRIDGE OR BUILD_ACCEPTANCE_TESTS)
    add_subdirectory(nltools)
ENDIF()

add_subdirectory(parameter-db)
add_subdirectory(playcontroller)
add_subdirectory(third-party)
