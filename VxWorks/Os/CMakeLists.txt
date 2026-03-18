####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(vxworks)
add_custom_target("${FPRIME_CURRENT_MODULE}")

register_fprime_module(
    Os_VxWorks_Shared
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/error.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/error.hpp"
  DEPENDS
    Fw_Time
    Fw_Types
)

register_fprime_implementation(
        Os_Console_VxWorks
    SOURCES
       "${CMAKE_CURRENT_LIST_DIR}/Console.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/ConsoleWait.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/DefaultConsole.cpp"
    HEADERS
       "${CMAKE_CURRENT_LIST_DIR}/Console.hpp"
    DEPENDS
        Fw_Types
    IMPLEMENTS
        Os_Console
)

register_fprime_implementation(
        Os_ConsoleNoWait_VxWorks
    SOURCES
       "${CMAKE_CURRENT_LIST_DIR}/Console.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/ConsoleNoWait.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/DefaultConsoleNoWait.cpp"
    HEADERS
       "${CMAKE_CURRENT_LIST_DIR}/ConsoleNoWait.hpp"
    DEPENDS
        Fw_Types
    IMPLEMENTS
        Os_Console
)

register_fprime_implementation(
        Os_Mutex_VxWorks
    SOURCES
       "${CMAKE_CURRENT_LIST_DIR}/Mutex.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/ConditionVariable.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/DefaultMutex.cpp"
    HEADERS
      "${CMAKE_CURRENT_LIST_DIR}/Mutex.hpp"
      "${CMAKE_CURRENT_LIST_DIR}/ConditionVariable.hpp"
    DEPENDS
        Fw_Types
        Os_VxWorks_Shared
    IMPLEMENTS
        Os_Mutex
)

register_fprime_implementation(
        Os_Queue_VxWorks
    SOURCES
       "${CMAKE_CURRENT_LIST_DIR}/Queue.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/DefaultQueue.cpp"
    HEADERS
       "${CMAKE_CURRENT_LIST_DIR}/Queue.hpp"
    DEPENDS
        Fw_Types
    IMPLEMENTS
        Os_Queue
)

register_fprime_implementation(
        Os_Task_VxWorks
    SOURCES
       "${CMAKE_CURRENT_LIST_DIR}/Task.cpp"
       "${CMAKE_CURRENT_LIST_DIR}/DefaultTask.cpp"
    HEADERS
       "${CMAKE_CURRENT_LIST_DIR}/Task.hpp"
    DEPENDS
        Fw_Time
        Fw_Types
        Os_VxWorks_Shared
    IMPLEMENTS
        Os_Task
)