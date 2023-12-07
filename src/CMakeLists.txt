add_executable(main)

add_compile_options(-Ofast)

# must match with pio filename and executable name from above
pico_generate_pio_header(main ${CMAKE_CURRENT_LIST_DIR}/pio/hsync.pio)
pico_generate_pio_header(main ${CMAKE_CURRENT_LIST_DIR}/pio/vsync.pio)
pico_generate_pio_header(main ${CMAKE_CURRENT_LIST_DIR}/pio/rgb.pio)

# must match with executable name and source file names
target_sources(main PRIVATE main.c lib/vga_graphics.c)

# must match with executable name
target_link_libraries(main PRIVATE pico_stdlib pico_divider pico_multicore pico_bootsel_via_double_reset hardware_pio hardware_dma hardware_adc hardware_irq hardware_clocks hardware_pll)

# must match with executable name
pico_add_extra_outputs(main)
