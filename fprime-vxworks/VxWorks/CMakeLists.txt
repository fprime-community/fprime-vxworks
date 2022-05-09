####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
cmake_minimum_required(VERSION 3.13)
cmake_policy(SET CMP0079 NEW)

get_directory_property(COMP_DEFS COMPILE_DEFINITIONS)

set(MOD_DEPS
        "${CMAKE_THREAD_LIBS_INIT}"
        Fw/Cfg
        Fw/Types
        Fw/Logger
        Utils/Hash
        )

if(COMP_DEFS MATCHES "TGT_OS_TYPE_VXWORKS")
  set(SOURCE_FILES
        "Queue.cpp"
        "Task.cpp"
        "Log.cpp"
        "WatchdogTimer.cpp"
        "InterruptLock.cpp"
        "TaskLock.cpp"
        "${FPRIME_FRAMEWORK_PATH}/Os/Posix/IntervalTimer.cpp"
        "Mutex.cpp"
        "RTSleep.c"
        "TaskId.cpp"
        "Event.cpp"
        "FileSystem.cpp"
        # Non-VxWorks basic support
        "${FPRIME_FRAMEWORK_PATH}/Os/Baremetal/File.cpp"
        "${FPRIME_FRAMEWORK_PATH}/Os/Baremetal/FileSystem.cpp"
      )
else()
  set(SOURCE_FILES
        "${FPRIME_FRAMEWORK_PATH}/Os/Baremetal/File.cpp"
     )
endif()

# If baremetal scheduler is set, remove the previous task files and add in the Baremetal variant
if (BAREMETAL_SCHEDULER)
    add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Baremetal/TaskRunner/")
    list(FILTER SOURCE_FILES EXCLUDE REGEX "Task\.cpp")
endif ()
register_fprime_module()

if(COMP_DEFS MATCHES "TGT_OS_TYPE_VXWORKS")
    add_dependencies(Os fprime-vxworks_VxWorks)
    target_link_libraries(Os PUBLIC fprime-vxworks_VxWorks)
    target_link_libraries(fprime-vxworks_VxWorks PUBLIC Os) #Power overwhelming
endif()



