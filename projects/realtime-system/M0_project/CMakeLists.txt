set (CPU "cortex-m0")

set(BLOBNAME "c15_lpc_m0")

# Optimisation / CPU and Warnings
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=${CPU} -Wall")

# Autoremove unused code
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdata-sections -ffunction-sections -Wl,--gc-section")

add_compile_options("-DCORE_M0")
add_compile_options("-DC15_VERSION_4")

file(GLOB_RECURSE SOURCES ./src/*.c ../nl_lib/nl_lib_com/*.c ../nl_lib/nl_lib_m0/*.c)

include_directories ("../nl_lib/nl_lib_m0/src")

add_executable(${BLOBNAME} ${SOURCES})

target_link_options(${BLOBNAME} PUBLIC -specs=nano.specs -specs=nosys.specs)
target_link_directories(${BLOBNAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ld)

set_target_properties(${BLOBNAME} PROPERTIES LINK_FLAGS "-TLPC4337_M0.ld")
set_target_properties(${BLOBNAME} PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS}")

add_custom_target(${BLOBNAME}.bin 
    COMMENT "Create ${BLOBNAME}.bin" 
    DEPENDS ${BLOBNAME} 
    COMMAND arm-none-eabi-objcopy -Obinary ${BLOBNAME} ${BLOBNAME}.bin)
