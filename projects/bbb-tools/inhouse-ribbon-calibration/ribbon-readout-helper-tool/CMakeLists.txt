cmake_minimum_required(VERSION 3.0)

project(ribbon-readout-helper-tool)

file(GLOB_RECURSE SOURCE_FILES ./*.bbmsg ./*.sh)
install(FILES ${SOURCE_FILES} DESTINATION share)


