####
# VxWorks-Posix.cmake:
#
# VxWorks posix platform file for standard vxworks targets.
####
register_fprime_target("${CMAKE_CURRENT_LIST_DIR}/../target/vxvalidate.cmake")

set(FPRIME_USE_POSIX ON)
set(FPRIME_HAS_SOCKETS ON)
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/types/Platform")

register_fprime_config(
        PlatformVxWorksPosix
   INTERFACE # No buildable files generated
   CHOOSES_IMPLEMENTATIONS
        Os_Console_Posix
        Os_File_Posix
        Os_Mutex_Posix
        Os_Generic_PriorityQueue
        Os_RawTime_Posix
        Os_Task_Posix
        Fw_StringFormat_snprintf
        # No VxWorks Implementation
        Os_Cpu_Stub
        Os_Memory_Stub
        Os_IntLock_VxWorks
)
target_compile_definitions(PlatformVxWorksPosix INTERFACE -DTGT_OS_TYPE_VXWORKS)
