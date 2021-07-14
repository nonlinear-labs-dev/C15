SET(DOCKERNAME nl-epc2-${ARCH_VERSION}-host-os)
SET(USER $ENV{USER})

message("Arch is ${ARCH_VERSION}")

SET(HOST_PACKAGES ${SHARED_PACKAGES} 
  nano vi mc arch-install-scripts dosfstools cmake make pkgconf gcc git wget unzip 
  jdk11-openjdk fuse-overlayfs fuse3 pacman-contrib typescript archiso
  linux mkinitcpio-archiso syslinux)

string(REPLACE ";" " " HOST_PACKAGES "${HOST_PACKAGES}")

SET(ARCH_INSTALL_PACKAGES packages_${ARCH_VERSION}.tar.gz)
SET(DOCKER_BUILD docker build --no-cache=true --build-arg tar=${ARCH_INSTALL_PACKAGES} --build-arg packages=${HOST_PACKAGES} -t ${DOCKERNAME} ${CMAKE_CURRENT_BINARY_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Dockerfile.in ${CMAKE_CURRENT_BINARY_DIR}/Dockerfile COPYONLY)

add_subdirectory(fetch-upstream-packages)

ADD_CUSTOM_COMMAND(
    COMMENT "Download NonLinux packages for ${ARCH_VERSION}"
    OUTPUT ${ARCH_INSTALL_PACKAGES}
    COMMAND wget https://nonlinearlabs.s3.eu-central-1.amazonaws.com/${ARCH_INSTALL_PACKAGES})
  
ADD_CUSTOM_COMMAND(
    COMMENT "Create docker container running a minimal ARCH based host os"
    OUTPUT .${DOCKERNAME}-created
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Dockerfile
    DEPENDS ${ARCH_INSTALL_PACKAGES}
    COMMAND ${DOCKER_BUILD}
    COMMAND touch .${DOCKERNAME}-created)

ADD_CUSTOM_TARGET(epc2-setup-host-os DEPENDS .${DOCKERNAME}-created DEPENDS ${ARCH_INSTALL_PACKAGES})
