cmake_minimum_required(VERSION 3.0)
project(C15-scripts)

OPTION(BUILD_BBB_SCRIPTS "Copy BeagleBone script files" Off)
OPTION(BUILD_EPC_SCRIPTS "Copy ePC script files" Off)

if(BUILD_BBB_SCRIPTS)
    add_subdirectory(bbb)
endif()

if(BUILD_EPC_SCRIPTS)
    add_subdirectory(epc)
endif()
