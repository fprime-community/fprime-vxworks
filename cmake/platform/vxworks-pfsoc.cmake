####
# VxWorks.cmake:
#
# VxWorks platform file for standard vxworks targets.
####
# choose_fprime_implementation(Os/File Os/File/Posix)
# choose_fprime_implementation(Os/Console Os/Console/Posix)
# choose_fprime_implementation(Os/Task Os/Task/Posix)
# choose_fprime_implementation(Os/Mutex Os/Mutex/Posix)
# choose_fprime_implementation(Os/Queue Os/Generic/PriorityQueue)
# choose_fprime_implementation(Os/RawTime Os/RawTime/Posix)
# choose_fprime_implementation(Os/Cpu Os/Cpu/Linux)
# choose_fprime_implementation(Os/Memory Os/Memory/Linux)

choose_fprime_implementation(Os/File Os/File/Stub)
choose_fprime_implementation(Os/Console Os/Console/Stub)
choose_fprime_implementation(Os/Task Os/Task/Stub)
choose_fprime_implementation(Os/Mutex Os/Mutex/Stub)
choose_fprime_implementation(Os/Queue Os/Queue/Stub)
choose_fprime_implementation(Os/RawTime Os/RawTime/Stub)
choose_fprime_implementation(Os/Cpu Os/Cpu/Stub)
choose_fprime_implementation(Os/Memory Os/Memory/Stub)

set(FPRIME_USE_POSIX ON)



## VxWorks is mostly Posix compliant. So turn this on.
#add_definitions(-D_POSIX_C_SOURCE)
#set(FPRIME_USE_POSIX ON)

# Add VxWorks specific headers into the system
set(VXWORKS_TYPES "${CMAKE_CURRENT_LIST_DIR}/types/VxWorks${CMAKE_SYSTEM_VERSION}")
IF(NOT EXISTS "${VXWORKS_TYPES}")
    message(FATAL_ERROR " VxWorks${CMAKE_SYSTEM_VERSION} types not found at ${VXWORKS_TYPES}.")
endif()

include_directories(SYSTEM "${VXWORKS_TYPES}")
