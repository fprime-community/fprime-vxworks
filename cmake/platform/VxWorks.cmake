# Use VxWorks common file
# Set VxWorks target
add_definitions(-DTGT_OS_TYPE_VXWORKS)
add_definitions(-D_WRS_KERNEL)

set(FPRIME_USE_BAREMETAL_SCHEDULE OFF)

# Add VxWorks specific headers into the system
include_directories(SYSTEM "${FPRIME_FRAMEWORK_PATH}/Fw/Types/VxWorks")
include_directories($ENV{WIND_BASE}/target/h $ENV{WIND_BASE}/target/h/wrn/coreip)
