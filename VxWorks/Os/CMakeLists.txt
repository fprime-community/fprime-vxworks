####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
add_custom_target("${FPRIME_CURRENT_MODULE}")

#### Shared Section ####
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/error.cpp"
)
set(HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/error.hpp"
)
set(MOD_DEPS Fw_Types)
register_fprime_module(Os_VxWorks_Shared)

# Set up VxWorks implementation
register_os_implementation("Console" VxWorks)
register_os_implementation("Mutex;ConditionVariable" VxWorks Os_VxWorks_Shared)
register_os_implementation("Queue" VxWorks)
register_os_implementation("Task" VxWorks Os_VxWorks_Shared Fw_Time)
