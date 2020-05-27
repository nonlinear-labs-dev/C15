SET(USER $ENV{USER})
SET(DOCKERNAME nl-cross-build-environment)

file(GLOB_RECURSE ALL_FILES_IN_PROJECT ${CMAKE_SOURCE_DIR}/projects/*)

file(GLOB_RECURSE REALTIME_SYSTEM_TARGET_FILES ${CMAKE_SOURCE_DIR}/projects/realtime-system/*)
list(REMOVE_ITEM ALL_FILES_IN_PROJECT EXCLUDE REGEX ${REALTIME_SYSTEM_TARGET_FILES})

file(GLOB_RECURSE SUPERVISOR_TARGET_FILES ${CMAKE_SOURCE_DIR}/projects/supervisor/*)
list(REMOVE_ITEM ALL_FILES_IN_PROJECT EXCLUDE REGEX ${SUPERVISOR_TARGET_FILES})

file(GLOB_RECURSE NONMAPS_TARGET_FILES ${CMAKE_SOURCE_DIR}/projects/NonMaps/target/*)
list(REMOVE_ITEM ALL_FILES_IN_PROJECT EXCLUDE REGEX ${NONMAPS_TARGET_FILES})

EXECUTE_PROCESS(OUTPUT_VARIABLE GROUP_ID COMMAND sh -c "id -g $USER")
EXECUTE_PROCESS(OUTPUT_VARIABLE USER_ID COMMAND sh -c "id -u $USER")

string(STRIP ${GROUP_ID} GROUP_ID)
string(STRIP ${USER_ID} USER_ID)

add_custom_command(
    COMMENT "Create Docker Container for the current User ${USER}"
    OUTPUT .docker-container-created 
    COMMAND docker build -t ${DOCKERNAME} ${CMAKE_CURRENT_SOURCE_DIR} DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Dockerfile 
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
    COMMENT "Clone nonlinux from github"
    OUTPUT .nonlinux-cloned 
    DEPENDS .filesystem-created
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/runInDocker.sh
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        "mkdir -p /fs"
        "mount -o loop /workdir/fs.ext4 /fs"
        "git clone -b nonlinear_2020.04 --single-branch https://github.com/nonlinear-labs-dev/nonlinux.git /fs/nonlinux"
        "cd /fs/nonlinux"
        "git status"
        "git pull origin nonlinear_2020.04"
        "git remote add rw git@github.com:nonlinear-labs-dev/nonlinux.git"
        "echo 1"
    COMMAND touch .nonlinux-cloned
)

add_custom_command(
    COMMENT "Pull nonlinux from github"
    OUTPUT .nonlinux-pulled 
    DEPENDS .nonlinux-cloned 
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/runInDocker.sh
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        "mkdir -p /fs"
        "mount -o loop /workdir/fs.ext4 /fs"
        "cd /fs/nonlinux"
        "git checkout nonlinear_2020.04"
        "git pull origin nonlinear_2020.04"
)

add_custom_command(
    COMMENT "Setup nonlinux"
    OUTPUT .nonlinux-setup
    DEPENDS .nonlinux-pulled .docker-container-created 
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/runInDocker.sh
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        "export HOME=/workdir/ccache"
        "mkdir -p /workdir/ccache"
        "mkdir -p /fs"
        "mount -o loop /workdir/fs.ext4 /fs"
        "cd /fs/nonlinux"
        "git pull origin nonlinear_2020.04"
        "make nonlinear_defconfig"
        "chown -R ${USER_ID}.${GROUP_ID} /workdir/ccache"
    COMMAND touch .nonlinux-setup
)

add_custom_command(
    COMMENT "Build BBB rootfs"
    OUTPUT ${CMAKE_BINARY_DIR}/build-tools/bbb/rootfs.tar.gz
    DEPENDS .nonlinux-setup
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/runInDocker.sh
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        "export HOME=/workdir/ccache"
        "mkdir -p /workdir/ccache"
        "mkdir -p /fs"
        "mount -o loop /workdir/fs.ext4 /fs"
        'rm -rf /fs/nonlinux/output/target/usr/C15'
        'rm -rf /fs/nonlinux/output/target/usr/C15-\*'
        "cd /fs/nonlinux"
        "make C15-clean-for-reconfigure"
        "make C15-rebuild"
        "make lpc_bb_driver-clean-for-reconfigure"
        "make lpc_bb_driver-rebuild"
        "make espi_driver-clean-for-reconfigure"
        "make espi_driver-rebuild"
        "make"
        "chown -R ${USER_ID}.${GROUP_ID} /workdir/ccache"
        "cp /fs/nonlinux/output/images/rootfs.tar.gz /workdir/"
        "chown ${USER_ID}.${GROUP_ID} /workdir/rootfs.tar.gz"
    COMMAND echo "The final rootfs is located at ${CMAKE_BINARY_DIR}/build-tools/bbb/rootfs.tar.gz"
)

add_custom_command(
    COMMENT "Build BBB update"
    OUTPUT ${CMAKE_BINARY_DIR}/nonlinear-bbb-update.tar
    DEPENDS ${CMAKE_BINARY_DIR}/build-tools/bbb/rootfs.tar.gz
    DEPENDS ${SOURCE_FILES}
    COMMAND ${CMAKE_SOURCE_DIR}/build-tools/create-c15-update/create-c15-update.sh
        "unused-epc"
        ${CMAKE_BINARY_DIR}/build-tools/bbb/rootfs.tar.gz
        "unused-lpc"
        "unused-lpc"
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        nonlinear-bbb-update
        bbb
)

add_custom_target(bbb-ota-update
    DEPENDS ${CMAKE_BINARY_DIR}/nonlinear-bbb-update.tar
    COMMAND ${CMAKE_SOURCE_DIR}/build-tools/c15-ota-update.sh
        ${CMAKE_BINARY_DIR}/nonlinear-bbb-update.tar
        \${UNIT_NAME}
        \${UNIT_PASSWORD}
    )

add_custom_target(bbb-rootfs DEPENDS ${CMAKE_BINARY_DIR}/build-tools/bbb/rootfs.tar.gz)
add_custom_target(bbb-prepare-build-dir DEPENDS .nonlinux-setup)

