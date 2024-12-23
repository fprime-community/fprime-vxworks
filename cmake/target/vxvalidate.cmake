####
# target/vxvalidate:
#
# A custom target used to detect when symbols are not defined when building DKMs.
#
####
set(VX_UNDEFINED_SYMBOLS "${CMAKE_CURRENT_LIST_DIR}/../utilities/undefined-symbols.py")

# No global functions needed
function(vxvalidate_add_global_target)
endfunction()

# Function `vxvalidate_add_deployment_target`:
#
# Used to establish a post-build command that will use the utilities/undefined-symbols.py script to detect any symbols
# that have not been properly set either in the build or by the kernel. This is done because DKMs do not detect
# undefined symbols until runtime.
#
# Args:
#   MODULE: name of the module to check for undefined symbols
####
function(vxvalidate_add_deployment_target MODULE)
    # Check if the kernel image exists
    if (NOT EXISTS "${VXWORKS_KERNEL_IMAGE_PATH}")
        message(WARNING "VxWorks kernel image not specified, or does not exist. Skipping undefined symbols check")
        return()
    endif()
    # Add command to detect undefined symbols
    add_custom_command(
        TARGET "${MODULE}" POST_BUILD
        COMMAND "${PYTHON}" "${VX_UNDEFINED_SYMBOLS}" "$<TARGET_FILE:${MODULE}>" "${VXWORKS_KERNEL_IMAGE_PATH}"
    )
endfunction()

# No module targets needed
function(vxvalidate_add_module_target)
endfunction()
