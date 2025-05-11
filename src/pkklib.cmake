
set(PKKLIB_PATH "${CMAKE_CURRENT_LIST_DIR}")

pico_sdk_init()

target_compile_definitions(${DEST} PRIVATE
    PSRAM_MUTEX=1
    #PSRAM_SPINLOCK=0
    PSRAM_ASYNC=0
    PSRAM_PIN_CS=20
    PSRAM_PIN_SCK=21
    PSRAM_PIN_MOSI=2
    PSRAM_PIN_MISO=3
)

add_subdirectory(${PKKLIB_PATH} pkklib)

target_link_libraries(${DEST}
    pkklib
    pico_stdlib 
    pico_multicore
    hardware_i2c 
    hardware_flash
    hardware_irq
    hardware_adc
    hardware_pwm
    hardware_spi
    hardware_dma
    hardware_exception
	hardware_pio
    )

pico_add_extra_outputs(${DEST})
pico_set_linker_script(${CMAKE_PROJECT_NAME} ${PKKLIB_PATH}/memmap_default_rp2040.ld)


# Move the generated files to the desired directory

set_target_properties(${DEST} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/out
)

add_custom_command(TARGET ${DEST} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rename
        ${CMAKE_CURRENT_BINARY_DIR}/${DEST}.bin
        ${CMAKE_CURRENT_SOURCE_DIR}/out/${DEST}.bin
)

add_custom_command(TARGET ${DEST} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rename
        ${CMAKE_CURRENT_BINARY_DIR}/${DEST}.uf2
        ${CMAKE_CURRENT_SOURCE_DIR}/out/${DEST}.uf2
)

add_custom_command(TARGET ${DEST} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rename
        ${CMAKE_CURRENT_BINARY_DIR}/${DEST}.hex
        ${CMAKE_CURRENT_SOURCE_DIR}/out/${DEST}.hex
)

add_custom_command(TARGET ${DEST} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rename
        ${CMAKE_CURRENT_BINARY_DIR}/${DEST}.dis
        ${CMAKE_CURRENT_SOURCE_DIR}/out/${DEST}.dis
)

add_custom_command(TARGET ${DEST} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rename
        ${CMAKE_CURRENT_BINARY_DIR}/${DEST}.elf.map
        ${CMAKE_CURRENT_SOURCE_DIR}/out/${DEST}.elf.map
)