set(BLOBNAME "main")

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-m4 -mthumb")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fno-builtin -nodefaultlibs -nostdlib")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-common -fmessage-length=0 -ffunction-sections -fdata-sections ")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-print-memory-usage")

add_compile_options("-DCORE_M4")

file(GLOB_RECURSE SOURCES ./src/*.c ../shared/*.c)

add_executable(${BLOBNAME} ${SOURCES} ../co-proc/co-proc.o)

target_include_directories (${BLOBNAME} PRIVATE "./src")
target_include_directories (${BLOBNAME} PRIVATE "../co-proc/src")
target_include_directories (${BLOBNAME} PRIVATE ${CMAKE_SOURCE_DIR}/projects/)

target_link_options(${BLOBNAME} PUBLIC -Wl,-Map,output.map -Wl,--gc-sections)
target_link_directories(${BLOBNAME} PUBLIC ${CMAKE_SOURCE_DIR}/projects/realtime-system/ld)

set_target_properties(${BLOBNAME} PROPERTIES LINKER_LANGUAGE C)
set_target_properties(${BLOBNAME} PROPERTIES LINK_FLAGS "-TC15_LPC4337_main.ld")
set_target_properties(${BLOBNAME} PROPERTIES COMPILE_FLAGS "${CMAKE_C_FLAGS}")

add_custom_target(${BLOBNAME}.bin 
    COMMENT "Create ${BLOBNAME}.bin" 
    DEPENDS ${BLOBNAME} 
    COMMAND arm-none-eabi-objcopy -Obinary ${BLOBNAME} ${BLOBNAME}.bin)
