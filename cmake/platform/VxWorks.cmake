####
# VxWorks.cmake:
#
# VxWorks platform file for standard vxworks targets.
####
# Set platform default for baremetal scheduler drivers
if (NOT DEFINED FPRIME_USE_BAREMETAL_SCHEDULER)
   set(FPRIME_USE_BAREMETAL_SCHEDULER OFF)
   #message(STATUS "Requiring thread library")
   #FIND_PACKAGE ( Threads REQUIRED )
endif()
choose_fprime_implementation(Os/File Os/File/Stub)
choose_fprime_implementation(Os/Task Os/Task/VxWorks)

## VxWorks is mostly Posix compliant. So turn this on.
#add_definitions(-D_POSIX_C_SOURCE)
#set(FPRIME_USE_POSIX ON)

# Add VxWorks specific headers into the system
set(VXWORKS_TYPES "${CMAKE_CURRENT_LIST_DIR}/types/VxWorks${CMAKE_SYSTEM_VERSION}")
IF(NOT EXISTS "${VXWORKS_TYPES}")
    message(FATAL_ERROR " VxWorks${CMAKE_SYSTEM_VERSION} types not found at ${VXWORKS_TYPES}.")
endif()

include_directories(SYSTEM "${VXWORKS_TYPES}")
