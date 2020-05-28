SET(USER $ENV{USER})
SET(DOCKERNAME nl-lpc-cross-build-environment)

SET(RT_SOURCES_DIR ${CMAKE_SOURCE_DIR}/projects/realtime-system)
file(GLOB_RECURSE ALL_FILES_IN_PROJECT ${RT_SOURCES_DIR}/*.c ${RT_SOURCES_DIR}/*.h ${RT_SOURCES_DIR}/*.txt)

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
    COMMENT "Build blob for LPC containing both cores"
    OUTPUT ${CMAKE_BINARY_DIR}/build-tools/lpc/projects/realtime-system/src/main/main.bin
    DEPENDS .docker-container-created
    DEPENDS ${ALL_FILES_IN_PROJECT}
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/runInDocker.sh
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        "cd /workdir"
        "cmake -DBUILD_AUDIOENGINE=Off -DBUILD_BBBB=Off -DBUILD_PLAYGROUND=Off -DBUILD_ONLINEHELP=Off -DBUILD_TEXT2SOLED=Off -DBUILD_TESTING=Off -DBUILD_REALTIME_SYSTEM=On /sources"
        "make main.bin"
        "chown -R ${USER_ID}.${GROUP_ID} /workdir/*"
)

IF(BUILD_BBB) # we need the BBB utilities
    add_custom_command(
        COMMENT "Build LPC update"
        OUTPUT ${CMAKE_BINARY_DIR}/nonlinear-lpc-update.tar
        DEPENDS ${CMAKE_BINARY_DIR}/build-tools/bbb/rootfs.tar.gz
        DEPENDS ${CMAKE_BINARY_DIR}/build-tools/lpc/projects/realtime-system/src/main/main.bin
        DEPENDS ${SOURCE_FILES}
        COMMAND ${CMAKE_SOURCE_DIR}/build-tools/create-c15-update/create-c15-update.sh
            "unused-epc"
            ${CMAKE_BINARY_DIR}/build-tools/bbb/rootfs.tar.gz
            ${CMAKE_BINARY_DIR}/build-tools/lpc/projects/realtime-system/src/main/main.bin
            ${CMAKE_BINARY_DIR}
            ${CMAKE_SOURCE_DIR}
            nonlinear-lpc-update
            lpc
    )

    add_custom_target(lpc-ota-update
        DEPENDS ${CMAKE_BINARY_DIR}/nonlinear-lpc-update.tar
        COMMAND ${CMAKE_SOURCE_DIR}/build-tools/c15-ota-update.sh
            ${CMAKE_BINARY_DIR}/nonlinear-lpc-update.tar
            \${UNIT_NAME}
            \${UNIT_PASSWORD}
        )
ENDIF()

add_custom_target(lpc-blobs
    DEPENDS ${CMAKE_BINARY_DIR}/build-tools/lpc/projects/realtime-system/src/main/main.bin
    )

