####
# VxWorks-common.cmake:
#
# Platform setup for all VxWorks based targets. This can be used for
# Sphinx, Sabertooth and other hosts.
####

# Set VxWorks target
add_definitions(-DTGT_OS_TYPE_VXWORKS)
add_definitions(-D_WRS_KERNEL)
add_definitions(-D__INCvsbConfig_h)

# Add VxWorks specific headers into the system
include_directories(SYSTEM "${FPRIME_FRAMEWORK_PATH}/Fw/Types/VxWorks")
include_directories($ENV{WIND_BASE}/target/h $ENV{WIND_BASE}/target/h/wrn/coreip)
