####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
cmake_minimum_required(VERSION 3.5)
project(Fprime-VxWorks C CXX)
include("${FPRIME_FRAMEWORK_PATH}/cmake/FPrime.cmake")
include("${FPRIME_FRAMEWORK_PATH}/cmake/FPrime-Code.cmake")
#include("${CMAKE_CURRENT_LIST_DIR}/fprime-vxworks.cmake")
