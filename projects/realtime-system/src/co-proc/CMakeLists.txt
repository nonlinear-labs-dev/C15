set(BLOBNAME "co-proc")

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-m0 -mthumb")

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -flto -ffat-lto-objects")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fno-builtin -nodefaultlibs -nostdlib")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-common -fmessage-length=0 -ffunction-sections -fdata-sections ")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-print-memory-usage")

add_compile_options("-DCORE_M0")

file(GLOB_RECURSE SOURCES ./src/*.c ../shared/*.c)

add_executable(${BLOBNAME} ${SOURCES})

target_include_directories (${BLOBNAME} PRIVATE "./src")
target_include_directories (${BLOBNAME} PRIVATE "../main/src")
target_include_directories (${BLOBNAME} PRIVATE ${CMAKE_SOURCE_DIR}/projects/)

target_link_options(${BLOBNAME} PUBLIC -Wl,-Map,output.map -Wl,--gc-sections)
target_link_directories(${BLOBNAME} PUBLIC ${CMAKE_SOURCE_DIR}/projects/realtime-system/ld)

set_target_properties(${BLOBNAME} PROPERTIES LINKER_LANGUAGE C)
set_target_properties(${BLOBNAME} PROPERTIES LINK_FLAGS "-TC15_LPC4337_co-proc.ld")
set_target_properties(${BLOBNAME} PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS}")

add_custom_target(${BLOBNAME}.o 
    COMMENT "Create ${BLOBNAME}.o" 
    DEPENDS ${BLOBNAME} 
    COMMAND arm-none-eabi-objcopy --target elf32-littlearm --verbose --strip-all --remove-section=.ARM.attributes --redefine-sym __vectors_start__=__vectors_start___core_m0app --keep-symbol __vectors_start___core_m0app --redefine-sym _data=__start_data_core_m0app --keep-symbol __start_data_core_m0app --redefine-sym _edata=__end_data_core_m0app --keep-symbol __end_data_core_m0app --remove-section=".bss*" --remove-section=".noinit*" --rename-section .ARM.exidx=".core_m0app.ARM.exidx" --rename-section .ARM.extab=".core_m0app.ARM.extab" --rename-section .text=".core_m0app" --rename-section .data=".core_m0app.data" --rename-section .data_RAM2=".core_m0app.data_RAM2" --rename-section .data_RAM3=".core_m0app.data_RAM3" --rename-section .data_RAM4=".core_m0app.data_RAM4" --rename-section .data_RAM5=".core_m0app.data_RAM5" ${BLOBNAME} ${BLOBNAME}.o)
