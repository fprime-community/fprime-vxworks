####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(vxworks)

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/tasklister_config")

register_fprime_module(
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/TaskLister.c"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/TaskLister.h"
  DEPENDS
    TaskLister-config
)
