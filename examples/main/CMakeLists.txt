set(TARGET_NAME_LINKER main)

add_executable(${TARGET_NAME_LINKER}
        ${TARGET_NAME_LINKER}.c
)

target_include_directories(${TARGET_NAME_LINKER} PRIVATE
        ./../
        )

target_compile_options(${TARGET_NAME_LINKER} PRIVATE -O2)

target_link_libraries(${TARGET_NAME_LINKER} PRIVATE
        cmsis_core
        pico_stdlib
        pico_flash
        hardware_spi
        hardware_dma
        hardware_irq
        hardware_flash
        hardware_watchdog
        ETHERNET_FILES
        IOLIBRARY_FILES
        BOARD_FILES
        CAN_FILES
        PLATFORM_HANDLER_FILES
        )

set_target_properties(${TARGET_NAME_LINKER} PROPERTIES COMPILE_FLAGS "-Wall")
pico_set_linker_script(${TARGET_NAME_LINKER} ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME_LINKER}.ld)

pico_enable_stdio_usb(${TARGET_NAME_LINKER} 1)
pico_enable_stdio_uart(${TARGET_NAME_LINKER} 0)
pico_add_extra_outputs(${TARGET_NAME_LINKER})