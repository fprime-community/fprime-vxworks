####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####
restrict_platforms(vxworks)

register_fprime_module(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/VxWorksUartDriver.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/VxWorksUartDriver.cpp"
  HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/VxWorksUartDriver.hpp"
  DEPENDS
    Os
)
