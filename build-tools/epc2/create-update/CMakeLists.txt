SET(DOCKERNAME nl-epc2-${ARCH_VERSION}-host-os)

ADD_CUSTOM_COMMAND(
    COMMENT "Build Nonlinear Labs binaries in a minmal NonLinux installation"
    DEPENDS epc2-setup-target-os
    DEPENDS createEPC2Update.sh
    DEPENDS createEPC2UpdateDockerCommands.sh
    OUTPUT epc2-update.tar
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/createEPC2Update.sh ${DOCKERNAME} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR} ${UPDATE_PACKAGES}
    )

ADD_CUSTOM_TARGET(epc2-create-update DEPENDS epc2-update.tar)