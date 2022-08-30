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

set(SOURCE_FILES
      "Queue.cpp"
      "Log.cpp"
      "InterruptLock.cpp"
      "TaskLock.cpp"
      "${FPRIME_FRAMEWORK_PATH}/Os/Posix/IntervalTimer.cpp"
      "Mutex.cpp"
      "TaskId.cpp"
      "Event.cpp"
    )

if(CMAKE_SYSTEM_VERSION MATCHES 6.[789])
  list(APPEND SOURCE_FILES
        "Task.cpp"
        "WatchdogTimer.cpp"
        "RTSleep.c"
      )
elseif(CMAKE_SYSTEM_VERSION MATCHES 7.[0-9])
  list(APPEND SOURCE_FILES
        #"Task.cpp"
        #"WatchdogTimer.cpp"
        #"RTSleep.c"
      )
endif()

register_fprime_module()
