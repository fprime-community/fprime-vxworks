####
# VxWorks.cmake:
#
# VxWorks platform file for standard vxworks targets.
####

include("${CMAKE_CURRENT_LIST_DIR}/VxWorks-common.cmake")

# Add VxWorks specific headers into the system
set(VXWORKS_TYPES "${CMAKE_CURRENT_LIST_DIR}/types")
include_directories(SYSTEM "${VXWORKS_TYPES}")
