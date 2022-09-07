# Use VxWorks common file
# Set VxWorks target
add_definitions(-DTGT_OS_TYPE_VXWORKS)
add_definitions(-D_WRS_KERNEL)

set(FPRIME_USE_BAREMETAL_SCHEDULE OFF)

# Add VxWorks specific headers into the system
set(VXWORKS_TYPES "${CMAKE_CURRENT_LIST_DIR}/types/VxWorks${CMAKE_SYSTEM_VERSION}")
IF(NOT EXISTS "${VXWORKS_TYPES}")
    message(FATAL_ERROR " VxWorks${CMAKE_SYSTEM_VERSION} types not found at ${VXWORKS_TYPES}.")
endif()

include_directories(SYSTEM "${VXWORKS_TYPES}")