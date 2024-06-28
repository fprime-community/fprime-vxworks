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

#### VxWorks/Os/Task Section ####
if(CMAKE_SYSTEM_VERSION MATCHES 6.[789])
  list(APPEND SOURCE_FILES
        "VxWorks6/Task.cpp"
      )
elseif(CMAKE_SYSTEM_VERSION MATCHES 7.[0-9])
  list(APPEND SOURCE_FILES
        "VxWorks7/Task.cpp"
      )
endif()


set(MOD_DEPS Os_VxWorks_Shared Fw/Time Os)
register_fprime_module(Os_Task_VxWorks)
register_fprime_implementation(Os/Task Os_Task_VxWorks "${CMAKE_CURRENT_LIST_DIR}/DefaultTask.cpp")

choose_fprime_implementation(Os/Task Os_Task_VxWorks)
