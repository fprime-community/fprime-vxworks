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
register_fprime_module(Os_VxWorks_Shared)

##########################################
####    VxWorks/Os/Task Section    ####
##########################################
list(APPEND SOURCE_FILES
    "Task.cpp"
)

register_fprime_module(Os_Task_VxWorks)
register_fprime_implementation(Os/Task Os_Task_VxWorks "${CMAKE_CURRENT_LIST_DIR}/DefaultTask.cpp")

##########################################
####    VxWorks/Os/Console Section    ####
##########################################
list(APPEND SOURCE_FILES
    "Console.cpp"
)

register_fprime_module(Os_Console_VxWorks)
register_fprime_implementation(Os/Console Os_Console_VxWorks "${CMAKE_CURRENT_LIST_DIR}/DefaultConsole.cpp")

##########################################
####    VxWorks/Os/Mutex Section    ####
##########################################
list(APPEND SOURCE_FILES
    "Mutex.cpp"
)

register_fprime_module(Os_Mutex_VxWorks)
register_fprime_implementation(Os/Mutex Os_Mutex_VxWorks "${CMAKE_CURRENT_LIST_DIR}/DefaultMutex.cpp")

# Finish setup

set(MOD_DEPS
      Os_VxWorks_Shared
      Fw/Time
      Os
)