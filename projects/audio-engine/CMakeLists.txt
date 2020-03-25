cmake_minimum_required(VERSION 3.0)
project(audio-engine)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set(LIBS alsa)

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
  target_include_directories(${PROJECT_NAME} PUBLIC ${${lib}_INCLUDE_DIRS})
  target_link_libraries(${PROJECT_NAME} PUBLIC ${${lib}_LIBRARIES})
  link_directories(${${lib}_LIBRARY_DIRS})
endforeach(lib)

include_directories(src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wdouble-promotion -march=native")

IF(DEV_PC)
    TARGET_COMPILE_DEFINITIONS(audio-engine PRIVATE test_inputModeFlag=1)
ENDIF(DEV_PC)

target_link_libraries(${PROJECT_NAME} PUBLIC pthread nltools)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/projects/nltools/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/projects/parameter-db/generated)

install(TARGETS ${PROJECT_NAME} DESTINATION ${C15_INSTALL_PATH}/${PROJECT_NAME})


configure_file(${PROJECT_SOURCE_DIR}/systemd/${PROJECT_NAME}.service.in ${PROJECT_BINARY_DIR}/systemd/${PROJECT_NAME}.service @ONLY)

install(FILES ${PROJECT_BINARY_DIR}/systemd/${PROJECT_NAME}.service DESTINATION ${SYSTEMD_SERVICES_INSTALL_DIR} COMPONENT init)
install(FILES ${PROJECT_SOURCE_DIR}/systemd/${PROJECT_NAME}.path DESTINATION ${SYSTEMD_SERVICES_INSTALL_DIR} COMPONENT init)

install(DIRECTORY DESTINATION ${SYSTEMD_SERVICES_INSTALL_DIR}/multi-user.target.wants)
install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${SYSTEMD_SERVICES_INSTALL_DIR}/${PROJECT_NAME}.path ${SYSTEMD_SERVICES_INSTALL_DIR}/multi-user.target.wants/${PROJECT_NAME}.path)")
