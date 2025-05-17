
# flash
add_custom_target(jlink_flash_boot
    COMMAND make -j14
    COMMAND JLinkExe -device ${MCU_NAME} -speed 4000 -if SWD -CommanderScript ${CMAKE_CURRENT_LIST_DIR}/jlink-flash-fw-standalone_boot.jlink
)

add_custom_target(jlink_flash_app
    COMMAND make -j14
    COMMAND JLinkExe -device ${MCU_NAME} -speed 4000 -if SWD -CommanderScript ${CMAKE_CURRENT_LIST_DIR}/jlink-flash-fw-standalone_app.jlink
)

# reset
add_custom_target(jlink_reset 
    COMMAND JLinkExe -device ${MCU_NAME} -speed 4000 -if SWD -CommanderScript ${CMAKE_CURRENT_LIST_DIR}/scripts/jlink-reset.jlink
)

# distclean
add_custom_target(distclean
    COMMAND rm -r *
    COMMAND cmake ..
)

# flash_clean
add_custom_target(flash_clean
    COMMAND rm -r *
    COMMAND cmake ..
    # COMMAND make jlink_flash
)

# GDB
add_custom_target(jlink_gdb_server 
    COMMAND JLinkGDBServer -device ${MCU_NAME} -if SWD
)

add_custom_target(jlink_gdb_boot_debug
    COMMAND ${ARM_GDB_EXECUTABLE} -x ${CMAKE_CURRENT_LIST_DIR}/scripts/jlink-debug.gdbinit ${PROJECT_BOOT_BINARY_DIR}/${BOOT_PROJECT}.elf
)

add_custom_target(jlink_gdb_app_debug
    COMMAND ${ARM_GDB_EXECUTABLE} -x ${CMAKE_CURRENT_LIST_DIR}/scripts/jlink-debug.gdbinit ${PROJECT_APP_BINARY_DIR}/${APP_PROJECT}.elf
)

# # string should be less than 30 bytes
# release_binary:
# 	printf "westonrobot-$(shell git rev-parse --short HEAD)\r\n" | cat - $(PROG).bin > weston-scout-$(shell git rev-parse --short HEAD).bin