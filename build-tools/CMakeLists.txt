add_subdirectory(bbb)
add_subdirectory(epc)
add_subdirectory(epc2)
add_subdirectory(playcontroller)

file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*)

add_custom_command(
    COMMENT "Build C15 update"
    OUTPUT ${CMAKE_BINARY_DIR}/nonlinear-c15-update.tar
    DEPENDS epc-create-update
    DEPENDS ${CMAKE_BINARY_DIR}/build-tools/epc/update.tar
    DEPENDS epc2-create-update
    DEPENDS ${CMAKE_BINARY_DIR}/build-tools/epc2/create-update/update.tar
    DEPENDS bbb-rootfs
    DEPENDS ${CMAKE_BINARY_DIR}/build-tools/bbb/rootfs.tar.gz
    DEPENDS playcontroller-blobs
    DEPENDS ${CMAKE_BINARY_DIR}/build-tools/playcontroller/projects/playcontroller/firmware/src/main/main.bin
    DEPENDS ${SOURCE_FILES}
    COMMAND ${CMAKE_SOURCE_DIR}/build-tools/create-c15-update/create-c15-update.sh
        ${CMAKE_BINARY_DIR}/build-tools/epc/update.tar
        ${CMAKE_BINARY_DIR}/build-tools/epc2/create-update/update.tar
        ${CMAKE_BINARY_DIR}/build-tools/bbb/rootfs.tar.gz
        ${CMAKE_BINARY_DIR}/build-tools/playcontroller/projects/playcontroller/firmware/src/main/main.bin
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        nonlinear-c15-update
        "playcontroller epc bbb"
)

add_custom_target(c15-update DEPENDS ${CMAKE_BINARY_DIR}/nonlinear-c15-update.tar)
add_custom_target(c15-ota-update
    DEPENDS ${CMAKE_BINARY_DIR}/nonlinear-c15-update.tar
    COMMAND ${CMAKE_SOURCE_DIR}/build-tools/c15-ota-update.sh
        ${CMAKE_BINARY_DIR}/nonlinear-c15-update.tar
        \${UNIT_NAME}
        \${UNIT_PASSWORD}
    )
