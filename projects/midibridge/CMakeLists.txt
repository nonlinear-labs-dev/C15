OPTION(BUILD_MIDIBRIDGE "Build the MIDI bridge firmware" Off)

IF(BUILD_MIDIBRIDGE)
    add_subdirectory(firmware)
ENDIF()
