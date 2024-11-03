add_subdirectory(kernel)
add_subdirectory(boot)

VNos_generate_target(src DEPENDS ${BOOT_DEPEND} ${KERNEL_DEPEND})
