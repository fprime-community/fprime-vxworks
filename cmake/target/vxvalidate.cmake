####
# target/vxvalidate:
#
# A custom target used to detect when symbols are not defined when building DKMs.
#
####

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
    set(VX_UNDEFINED_SYMBOLS "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../../fprime-vxworks/cmake/utilities/undefined-symbols.py")
    set(VXWORKS_KERNEL_IMAGE_PATH "$ENV{VXWORKS_KERNEL_IMAGE_PATH}")
    # Check if the kernel image exists
    if (NOT EXISTS "${VXWORKS_KERNEL_IMAGE_PATH}")
        message(WARNING "VxWorks kernel image not specified, or does not exist. Skipping undefined symbols check")
        return()
    endif()

    # Determine the nm to use. The DKM and kernel image are cross-compiled target
    # binaries (e.g. RISC-V 64-bit), so they must be parsed with the toolchain nm,
    # not the build host's nm (which may not read the target ELFs correctly and
    # would report spurious undefined symbols). Prefer CMake's detected CMAKE_NM.
    # A VXWORKS_NM cache/-D override is honored first for unusual toolchains.
    if (VXWORKS_NM)
        set(VX_NM "${VXWORKS_NM}")
    elseif (CMAKE_NM)
        set(VX_NM "${CMAKE_NM}")
    else()
        message(WARNING
            "Could not determine the toolchain 'nm' (CMAKE_NM is unset). "
            "Skipping the DKM undefined symbols check. Set -DVXWORKS_NM=/path/to/nm to enable it.")
        return()
    endif()

    # Add command to detect undefined symbols
    add_custom_command(
        TARGET "${MODULE}" POST_BUILD
        COMMAND "${PYTHON}" "${VX_UNDEFINED_SYMBOLS}" --nm "${VX_NM}" "$<TARGET_FILE:${MODULE}>" "${VXWORKS_KERNEL_IMAGE_PATH}"
    )
endfunction()

# No module targets needed
function(vxvalidate_add_module_target)
endfunction()
