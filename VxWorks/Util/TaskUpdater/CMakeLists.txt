####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(vxworks)

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/taskupdater_config")

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/TaskUpdater.c"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/TaskUpdater.h"
  DEPENDS
    TaskUpdater-config
)
