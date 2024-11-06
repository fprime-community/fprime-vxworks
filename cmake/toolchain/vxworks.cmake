####
# cmake/toolchain/vxwork.cmake:
#
# This is a generic toolchain for use with VxWorks. It leverages the CMake toolchain
# provided from the VxWorks Source Build. It requires the user to set a series of
# environment variables. FPRIME_WIND_SOURCE_BUILD is always set by the user. The
# others may be set by the user or sourced using `wrenv.linux`.
#
# FPRIME_VXWORKS_SOURCE_BUILD: path to the VxWorks source build for the project
# WIND_HOME: home directory for windriver install
# WIND_BASE: base directory of VxWorks install
# WIND_VX7_HOST_TYPE: host architecture, typically  x86_64-linux
# WIND_LLVM_COMPILER_PATH: (optional) path to windriver llvm directory.
####

# Set up the VSB path
if (NOT DEFINED ENV{FPRIME_WIND_SOURCE_BUILD})
    message(FATAL_ERROR "User must define environment variable FPRIME_WIND_SOURCE_BUILD")
# Check VxWorks toolchain.cmake available in VSB
elseif (NOT EXISTS "$ENV{FPRIME_WIND_SOURCE_BUILD}/mk/toolchain.cmake")
    message(FATAL_ERROR "VxWorks Source Build Malformed")
endif()
set(ENV{WIND_CC_SYSROOT} "$ENV{FPRIME_WIND_SOURCE_BUILD}")

# Check must-have environment variables
foreach (REQUIRED_ENVIRONMENT IN ITEMS WIND_HOME WIND_BASE)
    if (NOT DEFINED ENV{${REQUIRED_ENVIRONMENT}})
        message(FATAL_ERROR "User must define environment variable ${REQUIRED_ENVIRONMENT}")
    endif()
endforeach()


# Set LLVM PATH first with the explict path, then the versioned path, then with glob
if (DEFINED ENV{WIND_LLVM_PATH})
    set(WIND_LLVM_COMPILER_PATH "$ENV{WIND_LLVM_PATH}")
else()
    file(GLOB WIND_LLVM_COMPILER_PATH "$ENV{WIND_HOME}/compilers/llvm-*")
    list(LENGTH WIND_LLVM_COMPILER_PATH GLOB_COUNT)
    # Did not find ecactly one compiler
    if (NOT GLOB_COUNT EQUALS 1)
        message(FATAL_ERROR "Cannot detect llvm. Please set environment variable 'WIND_LLVM_COMPILER_PATH'")
    endif() 
endif()
# Determine the wr-cc compiler path
set(WIND_VXWORKS_COMPILER_PATH "$ENV{WIND_BASE}/host/$ENV{WIND_VX7_HOST_TYPE}/bin")
# Update path for the above two compilers
set(ENV{PATH} "${WIND_VXWORKS_COMPILER_PATH}:${WIND_LLVM_COMPILER_PATH}:$ENV{PATH}")

message(STATUS "[vxworks] VxWorks WIND_HOME: $ENV{WIND_HOME}")
message(STATUS "[vxworks] VxWorks WIND_BASE: $ENV{WIND_BASE}")
message(STATUS "[vxworks] VxWorks Compiler:  ${WIND_VXWORKS_COMPILER_PATH}")
message(STATUS "[vxworks] WindRiver LLVM:    ${WIND_LLVM_COMPILER_PATH}")

# Include the supplied CMake toolchain
include("$ENV{FPRIME_WIND_SOURCE_BUILD}/mk/toolchain.cmake")
