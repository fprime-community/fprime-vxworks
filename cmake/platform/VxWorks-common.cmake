####
# VxWorks-common.cmake:
#
# Platform setup for all VxWorks based targets. This can be used for
# Sphinx, Sabertooth and other hosts.
####

# Set VxWorks target
add_definitions(-DTGT_OS_TYPE_VXWORKS)
add_definitions(-D_WRS_KERNEL)

set(CMAKE_SYSTEM_NAME VxWorks7)

# Add VxWorks specific headers into the system
include_directories(SYSTEM "${FPRIME_FRAMEWORK_PATH}/Fw/Types/VxWorks")
include_directories(SYSTEM "/opt/tools/sabertooth/sabertooth-bsp/LeonVSB/krnl/h/public")
include_directories(SYSTEM "/opt/tools/sabertooth/sabertooth-bsp/LeonVSB/krnl/h/system")
include_directories("/opt/tools/sabertooth/sabertooth-bsp/LeonVSB/share/h")
