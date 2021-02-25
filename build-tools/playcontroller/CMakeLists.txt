SET(USER $ENV{USER})
SET(DOCKERNAME nl-playcontroller-cross-build-environment)

SET(RT_SOURCES_DIR ${CMAKE_SOURCE_DIR}/projects/playcontroller/firmware)
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
    COMMENT "Build blob for playcontroller containing both cores"
    OUTPUT ${CMAKE_BINARY_DIR}/build-tools/playcontroller/projects/playcontroller/firmware/src/main/main.bin
    DEPENDS .docker-container-created
    DEPENDS ${ALL_FILES_IN_PROJECT}
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/runInDocker.sh
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        "cd /workdir"
        "cmake -DBUILD_PLAYCONTROLLER=On /sources"
        "make main.bin"
        "chown -R ${USER_ID}.${GROUP_ID} /workdir/*"
)

add_custom_command(
    COMMENT "Build playcontroller update"
    OUTPUT ${CMAKE_BINARY_DIR}/nonlinear-playcontroller-update.tar
    DEPENDS ${CMAKE_BINARY_DIR}/build-tools/bbb/rootfs.tar.gz
    DEPENDS ${CMAKE_BINARY_DIR}/build-tools/playcontroller/projects/playcontroller/firmware/src/main/main.bin
    DEPENDS ${ALL_FILES_IN_PROJECT}
    COMMAND ${CMAKE_SOURCE_DIR}/build-tools/create-c15-update/create-c15-update.sh
        "unused-epc"
        "unused-epc2"
        ${CMAKE_BINARY_DIR}/build-tools/bbb/rootfs.tar.gz
        ${CMAKE_BINARY_DIR}/build-tools/playcontroller/projects/playcontroller/firmware/src/main/main.bin
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        nonlinear-playcontroller-update
        playcontroller
)

add_custom_target(playcontroller-ota-update
    DEPENDS ${CMAKE_BINARY_DIR}/nonlinear-playcontroller-update.tar
    COMMAND ${CMAKE_SOURCE_DIR}/build-tools/c15-ota-update.sh
        ${CMAKE_BINARY_DIR}/nonlinear-playcontroller-update.tar
        \${UNIT_NAME}
        \${UNIT_PASSWORD}
    )

add_custom_target(playcontroller-blobs
    DEPENDS ${CMAKE_BINARY_DIR}/build-tools/playcontroller/projects/playcontroller/firmware/src/main/main.bin
    )

