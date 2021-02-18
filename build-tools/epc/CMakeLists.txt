SET(USER $ENV{USER})
SET(DOCKERNAME nl-epc-build-environment)

file(GLOB_RECURSE ALL_FILES_IN_PROJECT ${CMAKE_SOURCE_DIR}/projects/*)
file(GLOB_RECURSE ALL_BUILD_TOOLS_FILES ${CMAKE_SOURCE_DIR}/build-tools/*)
file(GLOB_RECURSE NONMAPS_TARGET_FILES ${CMAKE_SOURCE_DIR}/projects/epc/web-ui/target/*)
file(GLOB_RECURSE PlaycontrollerX_FILES ${CMAKE_SOURCE_DIR}/projects/playcontroller/firmware/.metadata/*)
list(REMOVE_ITEM ALL_FILES_IN_PROJECT EXCLUDE REGEX ${NONMAPS_TARGET_FILES})
list(REMOVE_ITEM ALL_FILES_IN_PROJECT EXCLUDE REGEX ${PlaycontrollerX_FILES})
list(REMOVE_ITEM ALL_FILES_IN_PROJECT EXCLUDE REGEX CMakeLists.txt)
list(APPEND ALL_FILES_IN_PROJECT ${ALL_BUILD_TOOLS_FILES})

EXECUTE_PROCESS(OUTPUT_VARIABLE GROUP_ID COMMAND sh -c "id -g $USER")
EXECUTE_PROCESS(OUTPUT_VARIABLE USER_ID COMMAND sh -c "id -u $USER")
string(STRIP ${GROUP_ID} GROUP_ID)
string(STRIP ${USER_ID} USER_ID)
file(GLOB_RECURSE EPC_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/*)

add_custom_command(
    COMMENT "Create Docker Container"
    OUTPUT .docker-container-created
    COMMAND docker build -t ${DOCKERNAME} ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Dockerfile
    COMMAND touch .docker-container-created
)

add_custom_command(
    COMMENT "Create filesystem"
    OUTPUT  .filesystem-created
    COMMAND truncate -s 10G ${CMAKE_CURRENT_BINARY_DIR}/fs.ext4
    COMMAND mkfs.ext4 ${CMAKE_CURRENT_BINARY_DIR}/fs.ext4
    COMMAND touch .filesystem-created
)

add_custom_command(
    COMMENT "Download APLinux image to ${CMAKE_CURRENT_BINARY_DIR}/AP-Linux-V.4.0.iso"
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/AP-Linux-V.4.0.iso
    #COMMAND wget https://netix.dl.sourceforge.net/project/ap-linux/AP-Linux-V.4.0.iso
    COMMAND wget https://nonlinearlabs.s3.eu-central-1.amazonaws.com/AP-Linux-V.4.0.iso
)

add_custom_command(
    COMMENT "Download update packages"
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/NonLinux.pkg.tar.gz
    #COMMAND wget https://github.com/nonlinear-labs-dev/Audiophile2NonLinux/releases/download/1.0/NonLinux.pkg.tar.gz
    COMMAND wget https://nonlinearlabs.s3.eu-central-1.amazonaws.com/NonLinux.pkg.tar.gz
)

add_custom_command(
    COMMENT "Create NonLinux ISO Image"
    DEPENDS .docker-container-created
    DEPENDS .filesystem-created
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/AP-Linux-V.4.0.iso
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/NonLinux.pkg.tar.gz
    DEPENDS ${EPC_SCRIPTS}
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/NonLinuxAP.iso
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/runScriptInDocker.sh
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/createNonlinuxISO.sh
)

add_custom_command(
    COMMENT "Install system on virtual machine"
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/NonLinuxAP.iso
    DEPENDS .filesystem-created
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/disk.raw
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/runScriptInDocker.sh
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/installNonLinuxInfoVM.sh
)

add_custom_command(
  COMMENT "Download gwt"
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gwt-2.8.2.zip
  COMMAND wget https://goo.gl/pZZPXS -O ${CMAKE_CURRENT_BINARY_DIR}/gwt-2.8.2.zip)

add_custom_command(
    COMMENT "Create local rootfs"
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/AP-Linux-V.4.0.iso
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/NonLinux.pkg.tar.gz
    DEPENDS .docker-container-created
    DEPENDS .filesystem-created
    OUTPUT .nonlinux-local-rootfs
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/runScriptInDocker.sh
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/createLocalRootFS.sh
    COMMAND touch .nonlinux-local-rootfs
)


add_custom_command(
    COMMENT "Create ePC update"
    DEPENDS .nonlinux-local-rootfs
    DEPENDS ${ALL_FILES_IN_PROJECT}
    DEPENDS .filesystem-created
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/update.tar
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/runScriptInDocker.sh
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/createUpdate.sh
)

add_custom_command(
    COMMENT "Build install medium (to be run with prefix EPC_INSTALL_MEDIUM=/dev/sdX)"
    OUTPUT .nonlinux-install-medium
    DEPENDS .filesystem-created
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/NonLinuxAP.iso
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/createInstallMedium.sh
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/createInstallMedium.sh
        \${EPC_INSTALL_MEDIUM}
        ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(
    COMMENT "Build ePC update"
    OUTPUT ${CMAKE_BINARY_DIR}/nonlinear-epc-update.tar
    DEPENDS .filesystem-created
    DEPENDS epc-create-update
    DEPENDS ${CMAKE_BINARY_DIR}/build-tools/epc/update.tar
#    DEPENDS epc2-create-update
#    DEPENDS ${CMAKE_BINARY_DIR}/build-tools/epc2/create-update/update.tar
    DEPENDS bbb-rootfs
    DEPENDS bbb-rootfs
    DEPENDS ${ALL_FILES_IN_PROJECT}
    COMMAND ${CMAKE_SOURCE_DIR}/build-tools/create-c15-update/create-c15-update.sh
        ${CMAKE_BINARY_DIR}/build-tools/epc/update.tar
        ${CMAKE_BINARY_DIR}/build-tools/epc/update.tar
        ${CMAKE_BINARY_DIR}/build-tools/bbb/rootfs.tar.gz
        "unused-playcontroller"
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        nonlinear-epc-update
        epc
)

add_custom_target(epc-ota-update
    DEPENDS ${CMAKE_BINARY_DIR}/nonlinear-epc-update.tar
    DEPENDS .filesystem-created
    COMMAND ${CMAKE_SOURCE_DIR}/build-tools/c15-ota-update.sh
        ${CMAKE_BINARY_DIR}/nonlinear-epc-update.tar
        \${UNIT_NAME}
        \${UNIT_PASSWORD}
    )

add_custom_target(epc-install-medium DEPENDS .nonlinux-install-medium)
add_custom_target(epc-create-update DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/update.tar)
add_custom_target(epc-nonlinux-iso DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/NonLinuxAP.iso)
add_custom_target(epc-nonlinux-vm-installation DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/disk.raw)



