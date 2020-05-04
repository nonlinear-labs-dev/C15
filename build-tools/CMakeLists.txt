OPTION(BUILD_EPC "Build an ePC image" Off)
OPTION(BUILD_BBB "Build an BeagleBoneBlack image" Off)
OPTION(BUILD_LPC "Build blobs for LPC" Off)

IF(BUILD_BBB)
    add_subdirectory(bbb)
ENDIF()

IF(BUILD_EPC)
    add_subdirectory(epc)
ENDIF()

IF(BUILD_LPC)
    add_subdirectory(lpc)
ENDIF()


if(BUILD_BBB AND BUILD_EPC AND BUILD_LPC)

    file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*)

    add_custom_command(
        COMMENT "Build C15 update"
        OUTPUT ${CMAKE_BINARY_DIR}/nonlinear-c15-update.tar
        DEPENDS epc-create-update
        DEPENDS ${CMAKE_BINARY_DIR}/build-tools/epc/update.tar
        DEPENDS bbb-rootfs
        DEPENDS ${CMAKE_BINARY_DIR}/build-tools/bbb/rootfs.tar.gz
        DEPENDS lpc-blobs
        DEPENDS ${CMAKE_BINARY_DIR}/build-tools/lpc/projects/realtime-system/M0_project/c15_lpc_m0.bin
        DEPENDS ${CMAKE_BINARY_DIR}/build-tools/lpc/projects/realtime-system/M4_project/c15_lpc_m4.bin
        DEPENDS ${SOURCE_FILES}
        COMMAND ${CMAKE_SOURCE_DIR}/build-tools/create-c15-update/create-c15-update.sh
            ${CMAKE_BINARY_DIR}/build-tools/epc/update.tar
            ${CMAKE_BINARY_DIR}/build-tools/bbb/rootfs.tar.gz
            ${CMAKE_BINARY_DIR}/build-tools/lpc/projects/realtime-system/M0_project/c15_lpc_m0.bin
            ${CMAKE_BINARY_DIR}/build-tools/lpc/projects/realtime-system/M4_project/c15_lpc_m4.bin
            ${CMAKE_BINARY_DIR}
            ${CMAKE_SOURCE_DIR}
            nonlinear-c15-update
            "lpc epc bbb"
    )

    add_custom_target(c15-update DEPENDS ${CMAKE_BINARY_DIR}/nonlinear-c15-update.tar)
    add_custom_target(c15-ota-update 
        DEPENDS ${CMAKE_BINARY_DIR}/nonlinear-c15-update.tar
        COMMAND ${CMAKE_SOURCE_DIR}/build-tools/c15-ota-update.sh
            ${CMAKE_BINARY_DIR}/nonlinear-c15-update.tar
            \${UNIT_NAME}
            \${UNIT_PASSWORD}
        )

ENDIF()
