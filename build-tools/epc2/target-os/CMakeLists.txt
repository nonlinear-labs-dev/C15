SET(DOCKERNAME nl-epc2-${ARCH_VERSION}-host-os)

SET(INSTALL_PACKAGES ${TARGET_PACKAGES} ${SHARED_PACKAGES})
string(REPLACE ";" " " INSTALL_PACKAGES "${INSTALL_PACKAGES}")

ADD_CUSTOM_COMMAND(
    COMMENT "Build a minimal NonLinux installation in filesystem containers"
    DEPENDS epc2-setup-host-os
    DEPENDS createNonLinuxOS.sh
    DEPENDS createNonLinuxOSDockerCommands.sh
    OUTPUT rootfs.ext4
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/createNonLinuxOS.sh ${DOCKERNAME} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${INSTALL_PACKAGES} ${ARCH_VERSION}
)

ADD_CUSTOM_TARGET(epc2-setup-target-os DEPENDS rootfs.ext4)