SET(USER $ENV{USER})
SET(DOCKERNAME nl-midibridge-cross-build-environment)

SET(RT_SOURCES_DIR ${CMAKE_SOURCE_DIR}/projects/midibridge/firmware)
file(GLOB_RECURSE ALL_FILES_IN_PROJECT ${RT_SOURCES_DIR}/*.c ${RT_SOURCES_DIR}/*.h ${RT_SOURCES_DIR}/*.txt ${RT_SOURCES_DIR}/*.ld ${CMAKE_SOURCE_DIR}/projects/shared/* ${CMAKE_SOURCE_DIR}/build-tools/*)

EXECUTE_PROCESS(OUTPUT_VARIABLE GROUP_ID COMMAND sh -c "id -g $USER")
EXECUTE_PROCESS(OUTPUT_VARIABLE USER_ID COMMAND sh -c "id -u $USER")

string(STRIP ${GROUP_ID} GROUP_ID)
string(STRIP ${USER_ID} USER_ID)

add_custom_command(
    COMMENT "Create Docker Container"
    OUTPUT .docker-container-created
    COMMAND docker build -t ${DOCKERNAME} ${CMAKE_CURRENT_SOURCE_DIR} DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Dockerfile
    COMMAND touch .docker-container-created
)

add_custom_command(
    COMMENT "Build blob for midibridge containing both cores"
    OUTPUT ${CMAKE_BINARY_DIR}/build-tools/midibridge/projects/midibridge/firmware/src/main/main.bin
    DEPENDS .docker-container-created
    DEPENDS ${ALL_FILES_IN_PROJECT}
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/runInDocker.sh
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        "cd /workdir"
        "cmake -DBUILD_midibridge=On /sources"
        "make main.bin"
        "chown -R ${USER_ID}.${GROUP_ID} /workdir/*"
)

add_custom_target(midibridge-blobs
    DEPENDS ${CMAKE_BINARY_DIR}/build-tools/midibridge/projects/midibridge/firmware/src/main/main.bin
    )
