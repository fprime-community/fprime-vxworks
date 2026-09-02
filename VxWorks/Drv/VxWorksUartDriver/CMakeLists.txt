####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding files
# MOD_DEPS: (optional) module dependencies
#
####

set(MOD_DEPS Os)
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/VxWorksUartDriver.fpp"
    "${CMAKE_CURRENT_LIST_DIR}/VxWorksUartDriver.cpp"
)
register_fprime_module()
