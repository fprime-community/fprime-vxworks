####
# cmake/toolchain/vxwork.cmake:
#
# This is a generic toolchain for use with VxWorks. It leverages the CMake toolchain
# provided from the VxWorks Source Build. It requires the user to set a series of
# environment variables. WIND_CC_SYSROOT is always set by the user. The
# others may be set by the user or sourced using `wrenv.linux`.
#
# WIND_CC_SYSROOT: path to the VxWorks source build for the project
# WIND_HOME: home directory for windriver install
# WIND_BASE: base directory of VxWorks install
####

# Set up the VSB path
if (EXISTS ENV{WIND_CC_SYSROOT_OVERRIDE})
    set(WIND_CC_SYSROOT      ENV{WIND_CC_SYSROOT_OVERRIDE})
    set(ENV{WIND_CC_SYSROOT} $ENV{WIND_CC_SYSROOT_OVERRIDE})
else()
    set(WIND_CC_SYSROOT $ENV{WIND_CC_SYSROOT})
endif()

if (NOT DEFINED WIND_CC_SYSROOT)
    message(FATAL_ERROR "User must define environment or CMake variable WIND_CC_SYSROOT")
elseif (NOT EXISTS "$ENV{WIND_CC_SYSROOT}/mk/toolchain.cmake")
    message(FATAL_ERROR "VxWorks Source Build Malformed")
endif()

# Check must-have environment variables
foreach (REQUIRED_ENVIRONMENT IN ITEMS WIND_HOME WIND_BASE)
    if (NOT DEFINED ENV{${REQUIRED_ENVIRONMENT}})
        message(FATAL_ERROR "User must run with /path/to/vxworks/wrenv.linux")
    endif()
endforeach()

message(STATUS "[vxworks] VxWorks WIND_CC_SYSROOT: $ENV{WIND_CC_SYSROOT}")
message(STATUS "[vxworks] VxWorks WIND_HOME: $ENV{WIND_HOME}")
message(STATUS "[vxworks] VxWorks WIND_BASE: $ENV{WIND_BASE}")

# Include the supplied CMake toolchain
include("$ENV{WIND_CC_SYSROOT}/mk/toolchain.cmake")
