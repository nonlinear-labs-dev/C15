cmake_minimum_required(VERSION 3.2)

project(parameterDB)

include(GNUInstallDirs)

set(sourceDocument ${CMAKE_CURRENT_SOURCE_DIR}/src/ParameterList_1.7B.ods)
set(headerDocument ${CMAKE_CURRENT_SOURCE_DIR}/generated/parameter_list.h)

if(${sourceDocument} IS_NEWER_THAN ${headerDocument})
    message( WARNING "${sourceDocument} is newer than the generated header: ${headerDocument}! Please regenerate the headers from the spreadsheet!" )
endif()

