SET(USER $ENV{USER})
SET(DOCKERNAME nl-cross-build-environment-${USER})

file(GLOB_RECURSE ALL_FILES_IN_PROJECT ${CMAKE_SOURCE_DIR}/projects/*)

file(GLOB_RECURSE REALTIME_SYSTEM_TARGET_FILES ${CMAKE_SOURCE_DIR}/projects/realtime-system/*)
list(REMOVE_ITEM ALL_FILES_IN_PROJECT EXCLUDE REGEX ${REALTIME_SYSTEM_TARGET_FILES})

file(GLOB_RECURSE SUPERVISOR_TARGET_FILES ${CMAKE_SOURCE_DIR}/projects/supervisor/*)
list(REMOVE_ITEM ALL_FILES_IN_PROJECT EXCLUDE REGEX ${SUPERVISOR_TARGET_FILES})

if(NOT DOCKER_GROUP_ID)
    EXECUTE_PROCESS(OUTPUT_VARIABLE DOCKER_GROUP_ID COMMAND sh -c "getent group docker | cut -d: -f3")
endif()

message("Docker group id is ${DOCKER_GROUP_ID}")

EXECUTE_PROCESS(OUTPUT_VARIABLE USER_ID COMMAND sh -c "id -u $USER")
string(STRIP ${DOCKER_GROUP_ID} DOCKER_GROUP_ID)
string(STRIP ${USER_ID} USER_ID)

add_custom_command(
    COMMENT "Create Docker Container for the current User ${USER}"
    OUTPUT .docker-container-created 
    COMMAND docker build --build-arg user=${USER} --build-arg user_id=${USER_ID} --build-arg docker_group_id=${DOCKER_GROUP_ID} -t ${DOCKERNAME} ${CMAKE_CURRENT_SOURCE_DIR} DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Dockerfile 
    COMMAND touch .docker-container-created
)

add_custom_command(
    COMMENT "Clone nonlinux from github"
    OUTPUT .nonlinux-cloned 
    COMMAND git clone -b nonlinear_2020.01 --single-branch https://github.com/nonlinear-labs-dev/nonlinux.git 
    COMMAND touch .nonlinux-cloned
)

add_custom_command(
    COMMENT "Pull nonlinux from github"
    OUTPUT .nonlinux-pulled 
    DEPENDS .nonlinux-cloned 
    WORKING_DIRECTORY nonlinux
    COMMAND git checkout nonlinear_2020.01
    COMMAND git pull origin nonlinear_2020.01
)

add_custom_command(
    COMMENT "Setup nonlinux"
    OUTPUT .nonlinux-setup
    DEPENDS .nonlinux-pulled .docker-container-created 
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/runInDocker.sh
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        ${USER_ID}
        ${DOCKER_GROUP_ID}
        "export HOME=/workdir/ccache"
        "cd /workdir/nonlinux"
        "git pull origin nonlinear_2020.01"
        "make nonlinear_defconfig"
    COMMAND touch .nonlinux-setup
)

add_custom_command(
    COMMENT "Update C15 dir within nonlinux build directory"
    OUTPUT .nonlinux-update
    DEPENDS .nonlinux-setup
    DEPENDS ${ALL_FILES_IN_PROJECT}
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/runInDocker.sh
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        ${USER_ID}
        ${DOCKER_GROUP_ID}
        "rsync -av --delete /sources/projects /workdir/nonlinux/output/build/C15-master"
    COMMAND touch .nonlinux-update
)

add_custom_command(
    COMMENT "Build BBB rootfs"
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/nonlinux/output/images/rootfs.tar.gz
    DEPENDS .nonlinux-update
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/runInDocker.sh
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}
        ${USER_ID}
        ${DOCKER_GROUP_ID}
        "export HOME=/workdir/ccache"
        "cd /workdir/nonlinux"
        'rm -rf /workdir/nonlinux/output/target/usr/C15'
        'rm -rf /workdir/nonlinux/output/target/usr/C15-\*'
        "make C15-clean-for-reconfigure"
        "make C15-rebuild"
        "make"
    COMMAND echo "The final rootfs is located at ${CMAKE_CURRENT_BINARY_DIR}/nonlinux/output/images/rootfs.tar.gz"
)

add_custom_target(bbb-rootfs DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/nonlinux/output/images/rootfs.tar.gz)
add_custom_target(bbb-prepare-build-dir DEPENDS .nonlinux-setup)
