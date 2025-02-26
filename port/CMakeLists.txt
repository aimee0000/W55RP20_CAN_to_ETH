# ioLibrary_Driver
add_library(IOLIBRARY_FILES STATIC)

target_sources(IOLIBRARY_FILES PUBLIC
        ${PORT_DIR}/ioLibrary_Driver/src/w5x00_spi.c
        ${PORT_DIR}/ioLibrary_Driver/src/w5x00_gpio_irq.c
        )

if(${BOARD_NAME} STREQUAL W55RP20_EVB_PICO)
pico_generate_pio_header(IOLIBRARY_FILES ${PORT_DIR}/ioLibrary_Driver/src/wiznet_spi_pio.pio)

target_sources(IOLIBRARY_FILES PUBLIC 
        ${PORT_DIR}/ioLibrary_Driver/src/wiznet_spi_pio.c
        )
endif()

target_include_directories(IOLIBRARY_FILES PUBLIC
        ${WIZNET_DIR}/Ethernet
        ${PORT_DIR}/ioLibrary_Driver/inc
        ${PORT_DIR}/FreeRTOS-Kernel/inc
        ${PORT_DIR}
        )

target_link_libraries(IOLIBRARY_FILES PUBLIC
        pico_stdlib
        hardware_pio
        hardware_spi
        hardware_dma
        hardware_clocks
        FreeRTOS-Kernel-Heap4
        )

# timer
add_library(TIMER_FILES STATIC)

target_sources(TIMER_FILES PUBLIC
        ${PORT_DIR}/timer/timer.c
        )

target_include_directories(TIMER_FILES PUBLIC
        ${PORT_DIR}/timer
        )

target_link_libraries(TIMER_FILES PRIVATE
        pico_stdlib      
        )

# board
add_library(BOARD_FILES STATIC)

target_sources(BOARD_FILES PUBLIC
        ${PORT_DIR}/board/can/can.c
        ${PORT_DIR}/board/can/can_parser.c
        )

pico_generate_pio_header(BOARD_FILES ${PORT_DIR}/board/can/can.pio)

target_include_directories(BOARD_FILES PUBLIC
        ${PORT_DIR}/board/can
        ${PORT_DIR}/
        )

target_link_libraries(BOARD_FILES PRIVATE
        pico_stdlib      
        cmsis_core
        hardware_pio
        hardware_dma
        hardware_spi
        )

# CAN To Ethernet
add_library(CAN_FILES STATIC)

target_sources(CAN_FILES PUBLIC
        ${PORT_DIR}/can_to_ethernet/src/can_to_eth.c
        )

target_include_directories(CAN_FILES PUBLIC
        ${PORT_DIR}/can_to_ethernet/inc
        ${PORT_DIR}/
        )

target_link_libraries(CAN_FILES PRIVATE
        pico_stdlib      
        cmsis_core
        hardware_pio
        hardware_dma
        BOARD_FILES
        PLATFORM_HANDLER_FILES
        )


# Platform files
add_library(PLATFORM_HANDLER_FILES STATIC)

target_sources(PLATFORM_HANDLER_FILES PUBLIC
        ${PORT_DIR}/platform_handler/src/canHandler.c
        ${PORT_DIR}/platform_handler/src/configData.c
        ${PORT_DIR}/platform_handler/src/flashHandler.c
        ${PORT_DIR}/platform_handler/src/httpHandler.c
        ${PORT_DIR}/platform_handler/src/httpParser.c
        ${PORT_DIR}/platform_handler/src/httpServer.c
        ${PORT_DIR}/platform_handler/src/httpUtil.c
)

target_include_directories(PLATFORM_HANDLER_FILES PUBLIC
        ${PORT_DIR}/platform_handler/inc
        ${PORT_DIR}
)

target_link_libraries(PLATFORM_HANDLER_FILES PUBLIC
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
)