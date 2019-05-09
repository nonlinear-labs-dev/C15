#!/bin/bash

if [ $# -lt 1 ]; then
	printf "Usage:"
	printf " $0 <PATH_TO_BUILDROOT>\n"
	exit
fi

BASE_DIR=$(readlink -f  $1)
TARGET_DIR=${BASE_DIR}/output/target
BUILD_DIR=${BASE_DIR}/output/build/playground-HEAD
STAGING_DIR=${BASE_DIR}/output/staging
HOST_DIR=/root/development/buildroot_assets/host

GWT_COMPILER_DIR=${HOST_DIR}/$(ls ${HOST_DIR} | grep gwt | sort -n | tail -n1)

if [[ -z $GWT_COMPILER_DIR ]];
then
	echo "no GWT Compiler found!"
	exit 2
fi

mkdir -p build
cd build
${HOST_DIR}/usr/bin/cmake \
	-DPKG_CONFIG_USE_CMAKE_PREFIX_PATH=${HOST_DIR} \
	-DCMAKE_PREFIX_PATH=${HOST_DIR} \
	-DCMAKE_TOOLCHAIN_FILE="${HOST_DIR}/usr/share/buildroot/toolchainfile.cmake" \
	-DCMAKE_COLOR_MAKEFILE=OFF \
	-DBUILD_DOC=OFF \
	-DBUILD_DOCS=OFF \
	-DBUILD_EXAMPLE=OFF \
	-DBUILD_EXAMPLES=OFF \
	-DBUILD_TEST=OFF \
	-DBUILD_TESTS=OFF \
	-DBUILD_TESTING=OFF \
	-DBUILD_SHARED_LIBS=ON \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX="${TARGET_DIR}" \
	-DJARSDIR="${GWT_COMPILER_DIR}" \
	.. && \
make -j8 && \
make install
