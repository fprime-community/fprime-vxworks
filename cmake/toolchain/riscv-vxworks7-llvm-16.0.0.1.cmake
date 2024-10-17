# Set system name
set(CMAKE_SYSTEM_NAME VxWorks)
set(CMAKE_SYSTEM_VERSION 7.0)
set(CMAKE_SYSTEM_PROCESSOR pfsoc)
set(FPRIME_PLATFORM vxworks-pfsoc)

set(WIND_BASE $ENV{WIND_BASE})
set(VSB_HOME $ENV{VSB_HOME})

include_directories(SYSTEM "${VSB_HOME}/krnl/h/public")
include_directories(SYSTEM "${VSB_HOME}/krnl/h/system")
include_directories("${VSB_HOME}/share/h")

add_definitions(-DBUILD_RAD750)

set(WINDRIVER_COMPILER_ROOT "${WIND_BASE}/../../compilers/llvm-16.0.0.1/LINUX64")
# Check toolchain directory exists
IF(NOT EXISTS "${WINDRIVER_COMPILER_ROOT}")
    message(FATAL_ERROR " Windriver compilers not found at ${WINDRIVER_COMPILER_ROOT}.")
endif()
message(STATUS "Using VxWorks compilers at: ${WINDRIVER_COMPILER_ROOT}")
# specify the cross compiler
set(CMAKE_C_COMPILER "${WINDRIVER_COMPILER_ROOT}/bin/clang")
set(CMAKE_ASM_COMPILER "${WINDRIVER_COMPILER_ROOT}/bin/clang")
set(CMAKE_CXX_COMPILER "${WINDRIVER_COMPILER_ROOT}/bin/clang")
set(CMAKE_AR "${WINDRIVER_COMPILER_ROOT}/bin/arppc")
set(CMAKE_RANLIB "${WINDRIVER_COMPILER_ROOT}/bin/ranlibppc")
set(CMAKE_C_ARCHIVE_CREATE "${CMAKE_AR} cr <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_ARCHIVE_APPEND "${CMAKE_AR} r  <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_ARCHIVE_FINISH "${CMAKE_RANLIB} <TARGET>")
set(CMAKE_CXX_ARCHIVE_CREATE "${CMAKE_C_ARCHIVE_CREATE}")
set(CMAKE_CXX_ARCHIVE_APPEND "${CMAKE_C_ARCHIVE_APPEND}")
set(CMAKE_CXX_ARCHIVE_FINISH "${CMAKE_C_ARCHIVE_FINISH}")
set(CMAKE_ASM_ARCHIVE_APPEND "${CMAKE_C_ARCHIVE_APPEND}")
set(CMAKE_ASM_ARCHIVE_FINISH "${CMAKE_C_ARCHIVE_FINISH}")
set(CMAKE_ASM_ARCHIVE_CREATE "${CMAKE_C_ARCHIVE_CREATE}")

set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CURRENT_LIST_DIR}/../helpers/VxWorks/VxWorksLink.py -o <TARGET> <OBJECTS> <LINK_LIBRARIES>")
set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_CURRENT_LIST_DIR}/../helpers/VxWorks/VxWorksLink.py -o <TARGET> <OBJECTS> <LINK_LIBRARIES>")
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_ASM_COMPILER_WORKS 1)

set(COMPILER_COMMON_FLAGS
    "-DTGT_OS_TYPE_VXWORKS \
    -DTGT_OS_TYPE_VXWORKS7 \
    -D_VSB_CONFIG_FILE=\\\"/home/tcanham/source/VxWorks-BSPs/VxWorks7/VxWorks7-PFSOC/vsb_pfsoc/h/config/vsbConfig.h\\\" \
    --target=riscv64 \
    -march=rv64imafdc \
    -mabi=lp64d \
    -mcmodel=medlow \
    -mno-implicit-float \
    -std=c11 \
    -D__vxworks \
    -D__VXWORKS__ \
    -D__ELF__ \
    -D_HAVE_TOOL_XTORS  \
    -nostdlibinc \
    -nostdinc++ \
    -ftls-model=local-exec \
    -fno-builtin \
    -fno-strict-aliasing   \
    -D_USE_INIT_ARRAY \
    -fwrapv \
    -mllvm \
    -two-entry-phi-node-folding-threshold=2 \
    -fno-unwind-tables \
    -fno-asynchronous-unwind-tables \
    -Wall \
    -Wconversion \
    -Wno-sign-conversion \
    -Wno-unused-but-set-variable \
    -Wno-deprecated-non-prototype    \
    -MD \
    -MP \
    -mno-relax \
    -DCPU=_VX_RISCV \
    -DTOOL_FAMILY=llvm \
    -DTOOL=llvm \
    -D_WRS_KERNEL \
    -D_WRS_VX_SMP \
    -D_WRS_CONFIG_SMP \
    "
)
    
set(CMAKE_C_FLAGS
    "${COMPILER_COMMON_FLAGS} \
    -std=c99 \
    -Werror-implicit-function-declaration \
    -Wstrict-prototypes"
    )  

# Rejected by clang
#     -fno-cond-mismatch -pedantic \



set(CMAKE_CXX_FLAGS
    "${COMPILER_COMMON_FLAGS} \
    -std=c++14   \
	-fno-rtti \
    -Wnon-virtual-dtor"
    )  

# where is the target environment
set(CMAKE_FIND_ROOT_PATH  "${WINDRIVER_TOOL_ROOT}")

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
