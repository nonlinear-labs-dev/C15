cmake_minimum_required(VERSION 3.0)
project(audio-engine)

SET(AE_LIB_NAME ${PROJECT_NAME}-lib)

find_systemd()
enable_ccache()
prepare_nltools()

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
list(REMOVE_ITEM SOURCE_FILES EXCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_library(${AE_LIB_NAME} STATIC ${SOURCE_FILES})
add_nltools(${AE_LIB_NAME})

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${AE_LIB_NAME})

set(LIBS alsa flac)

find_package(OpenMP)

IF(OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
ENDIF()

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

foreach(lib ${LIBS})
  pkg_check_modules(${lib} REQUIRED ${lib})
  target_include_directories(${AE_LIB_NAME} INTERFACE ${${lib}_INCLUDE_DIRS})
  target_link_libraries(${AE_LIB_NAME} INTERFACE ${${lib}_LIBRARIES})
  link_directories(${${lib}_LIBRARY_DIRS})
endforeach(lib)

include_directories(src)

IF(DEV_PC)
    TARGET_COMPILE_DEFINITIONS(${AE_LIB_NAME} PRIVATE test_inputModeFlag=1)
ENDIF(DEV_PC)

target_link_libraries(${AE_LIB_NAME} PUBLIC pthread)
target_include_directories(${AE_LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/projects/shared/parameter-db/generated)

add_subdirectory(tests)

install(TARGETS ${PROJECT_NAME} DESTINATION ${C15_INSTALL_PATH}/${PROJECT_NAME})

configure_file(${PROJECT_SOURCE_DIR}/systemd/${PROJECT_NAME}.service.in ${PROJECT_BINARY_DIR}/systemd/${PROJECT_NAME}.service @ONLY)
configure_file(${PROJECT_SOURCE_DIR}/systemd/fixCpuBindings.service.in ${PROJECT_BINARY_DIR}/systemd/fixCpuBindings.service @ONLY)

install(FILES ${PROJECT_BINARY_DIR}/systemd/${PROJECT_NAME}.service DESTINATION ${SYSTEMD_SERVICES_INSTALL_DIR} COMPONENT init)
install(FILES ${PROJECT_BINARY_DIR}/systemd/fixCpuBindings.service DESTINATION ${SYSTEMD_SERVICES_INSTALL_DIR} COMPONENT init)

install(DIRECTORY DESTINATION ${SYSTEMD_SERVICES_INSTALL_DIR}/multi-user.target.wants)
install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ../fixCpuBindings.service $ENV{DESTDIR}/${SYSTEMD_SERVICES_INSTALL_DIR}/multi-user.target.wants/fixCpuBindings.service)")
install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ../audio-engine.service $ENV{DESTDIR}/${SYSTEMD_SERVICES_INSTALL_DIR}/multi-user.target.wants/audio-engine.service)")
