SET(DOCKERNAME nl-epc2-${ARCH_VERSION}-host-os)
SET(USER $ENV{USER})

message("Arch is ${ARCH_VERSION}")

SET(DOCKER_BUILD docker build --no-cache=true --build-arg arch_version=${ARCH_VERSION} --build-arg packages=${SHARED_PACKAGES} -t ${DOCKERNAME} ${CMAKE_CURRENT_SOURCE_DIR})
SET(ARCH_INSTALL_PACKAGES packages_${ARCH_VERSION}.tar.gz)

ADD_CUSTOM_COMMAND(
    COMMENT "Create docker container running a minimal ARCH based host os"
    OUTPUT .${DOCKERNAME}-created
    DEPENDS Dockerfile
    COMMAND ${DOCKER_BUILD}
    COMMAND touch .${DOCKERNAME}-created)

ADD_CUSTOM_COMMAND(
    COMMENT "Download NonLinux packages for ${ARCH_VERSION}"
    OUTPUT ${ARCH_INSTALL_PACKAGES}
    COMMAND wget https://nonlinearlabs.s3.eu-central-1.amazonaws.com/${ARCH_INSTALL_PACKAGES})

ADD_CUSTOM_COMMAND(
    COMMENT "Provide well-defined package selection"
    OUTPUT .${DOCKERNAME}-packages
    DEPENDS ${ARCH_INSTALL_PACKAGES}
    COMMAND rm -rf pac-cache
    COMMAND rm -rf pac-conf
    COMMAND tar -xvf ${ARCH_INSTALL_PACKAGES}
    COMMAND touch .${DOCKERNAME}-packages)

# Download the latest packages from official Arch repository - should only be used for creating new NonLinux releases
# for example if new hardware has to be integrated. Normally, the build system has to download prepared, well-defined
# archives of packages to use. This target creates those archives for uploading.
ADD_CUSTOM_COMMAND(
    COMMENT "Create a new NonLinux release (a tar of packages to use) using the latest versions of ARCH packages"
    OUTPUT .fetch-upstream-packages
    DEPENDS .${DOCKERNAME}-created
    DEPENDS createNonLinuxReleasePackages.sh
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/createNonLinuxReleasePackages.sh 
        ${DOCKERNAME} 
        ${CMAKE_CURRENT_BINARY_DIR}/${ARCH_INSTALL_PACKAGES}
        ${ALL_PACKAGES})
 
ADD_CUSTOM_TARGET(epc2-fetch-upstream-packages DEPENDS .fetch-upstream-packages)
ADD_CUSTOM_TARGET(epc2-setup-host-os DEPENDS .${DOCKERNAME}-created DEPENDS .${DOCKERNAME}-packages)
