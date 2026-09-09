####
# VxWorks.cmake:
#
# VxWorks platform file for standard vxworks targets.
####
register_fprime_target("${CMAKE_CURRENT_LIST_DIR}/../target/vxvalidate.cmake")

set(FPRIME_HAS_SOCKETS ON)
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/types/Platform")

register_fprime_config(
        PlatformVxWorks
   INTERFACE # No buildable files generated
   CHOOSES_IMPLEMENTATIONS
        Os_Console_VxWorks
        Os_Mutex_VxWorks
        Os_Queue_VxWorks
        Os_Task_VxWorks
        Fw_StringFormat_snprintf
        Fw_StringScan_sscanf
        Fw_AssertHook_fputs
        # No VxWorks Implementation
        Os_Cpu_Stub
        Os_File_Stub
        Os_Memory_Stub
        Os_RawTime_Stub
        Os_CountingSemaphore_Stub
   BASE_CONFIG
)

target_compile_definitions(PlatformVxWorks INTERFACE -DTGT_OS_TYPE_VXWORKS)
