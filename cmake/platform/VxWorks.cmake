####
# VxWorks.cmake:
#
# These files specify build flags, compiler directives, and must specify an include
# directory for system includes like "PlatformTypes.hpp".
#
# **Note:** If the user desires to set compiler paths, and other CMake toolchain settings, a
#           toolchain file should be constructed. See: [toolchain.md](toolchain.md)
#
# ### Platform File Loading ###
#
# The user rarely needs to specify a platform file directly. It will be specified based on the data
# in the chosen Toolchain file, or by the CMake system itself. However, if the user wants to control
# which platform file is used, the load is specified by the following rules:
#
# If the user specifies a CMake Toolchain file, then the platform file `${CMAKE_SYSTEM_NAME}.cmake`
# will be used. `${CMAKE_SYSTEM_NAME}` is set in the toolchain file and is typically set to a name like Linux, or Darwin
# but may be more specific if required.
#
# Otherwise, CMake sets the `${CMAKE_SYSTEM_NAME}` automatically to be that of the Host system, and that platform
# will be used. e.g. when building on Linux, the platform file "Linux.cmake" will be used.
#
# ### Filling In CMake Platform by Example ###
#
# F prime platform files are used to set F prime specific settings. This allows the user to control
# some aspects of the F prime build at the top-level. This means setting global include directories
# compiler definitions for the platform, threading libraries, etc. The bare-minimum platform file
# should specify an include directory for "PlatformTypes.hpp" and a threading library if using
# active components with OS supported threads. This can be done with the following lines:
#
# ```
# FIND_PACKAGE ( Threads REQUIRED )
# include_directories(SYSTEM "${FPRIME_FRAMEWORK_PATH}/Fw/Types/Linux")
# ```
#
####

## STEP 1: Specify the OS type include directive i.e. LINUX or DARWIN
add_definitions(-DTGT_OS_TYPE_VXWORKS)

# STEP 3: Specify a directory containing the "PlatformTypes.h" headers, as well
#         as other system headers. Other global headers can be placed here.
#         Note: Typically, the Linux directory is a good default, as it grabs
#         standard types from <cstdint>.
include_directories(SYSTEM "${CMAKE_CURRENT_LIST_DIR}/types")
