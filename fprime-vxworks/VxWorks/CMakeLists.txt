####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(VxWorks)
add_custom_target("${FPRIME_CURRENT_MODULE}")

#### Shared Section ####
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/error.cpp"
)
set(HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/error.hpp"
)

#### Os/Task/Posix Section ####
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Task.cpp"
)
set(HEADER_FILES
    "${CMAKE_CURRENT_LIST_DIR}/Task.hpp"
)

set(MOD_DEPS Fw/Time Os)
register_fprime_module(Os_Task_VxWorks)
register_fprime_implementation(Os/Task Os_Task_VxWorks "${CMAKE_CURRENT_LIST_DIR}/DefaultTask.cpp")

set(UT_MOD_DEPS
        Os
        STest
)
choose_fprime_implementation(Os/Task Os_Task_VxWorks)
