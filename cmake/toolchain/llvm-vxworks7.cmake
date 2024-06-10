# Set system name
set(CMAKE_SYSTEM_NAME VxWorks)
set(CMAKE_SYSTEM_VERSION 7.0)

set(VSB_HOME "$ENV{VSB_HOME}")
set(WIND_BASE "$ENV{WIND_BASE}")
set(WIND_COMP "$ENV{WIND_COMP}")


## specify the cross compiler
set(CMAKE_C_COMPILER "${WIND_COMP}/ccllvm")
set(CMAKE_CXX_COMPILER "${WIND_COMP}/ccllvm")
set(CMAKE_AR "${WIND_COMP}/arllvm")
set(CMAKE_RANLIB "${WIND_COMP}/ranlibllvm")
set(CMAKE_C_ARCHIVE_CREATE "${CMAKE_AR} cr <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_ARCHIVE_APPEND "${CMAKE_AR} r  <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_ARCHIVE_FINISH "${CMAKE_RANLIB} <TARGET>")
set(CMAKE_CXX_ARCHIVE_CREATE "${CMAKE_C_ARCHIVE_CREATE}")
set(CMAKE_CXX_ARCHIVE_APPEND "${CMAKE_C_ARCHIVE_APPEND}")
set(CMAKE_CXX_ARCHIVE_FINISH "${CMAKE_C_ARCHIVE_FINISH}")

set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CURRENT_LIST_DIR}/../helpers/VxWorks/VxWorksLink.py -o <TARGET> <OBJECTS> <LINK_LIBRARIES>")
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_ASM_COMPILER_WORKS 1)

#set(COMPILER_COMMON_FLAGS
#    "-DTGT_OS_TYPE_VXWORKS \
#    -DTGT_OS_TYPE_VXWORKS7 \
#    -D__vxworks \
#    -D__VXWORKS__ \
#    -D__ELF__  \
#    -D_HAVE_TOOL_XTORS \
#    -D_VSB_CONFIG_FILE=\\\"${VSB_HOME}/h/config/vsbConfig.h\\\" \
#    -D_WRS_KERNEL \
#    -DTOOL_FAMILY=gnu -DTOOL=gnu \
#    -Wall -Wextra \
#    -fno-builtin  \
#    -Wno-unused-parameter \
#    -Wno-long-long \
#    -Wconversion \
#    -Wno-sign-conversion \
#    -ansi \
#    -ftls-model=local-exec  \
#    -fno-zero-initialized-in-bss \
#    -D_WRS_VX_SMP \
#    -D_WRS_CONFIG_SMP \
#    -D_WRS_CONFIG_FOPEN_MAX_KERNEL=50 \
#    -fno-builtin \
#    -fno-strict-aliasing \
#    -D_USE_INIT_ARRAY -Wall"
#    )
#
#set(CMAKE_C_FLAGS
#    "${COMPILER_COMMON_FLAGS} \
#    -std=c99 \
#    -pedantic \
#    -Werror-implicit-function-declaration \
#    -Wstrict-prototypes"
#    )
#
#set(CMAKE_CXX_FLAGS
#    "${COMPILER_COMMON_FLAGS} \
#    -std=c++14   \
#    -fno-rtti \
#    -Wnon-virtual-dtor"
#    )
#
set(CMAKE_C_FLAGS
    "${CFLAGS}"
    )
set(CMAKE_CXX_FLAGS
    "${CFLAGS}"
)

# where is the target environment
set(CMAKE_FIND_ROOT_PATH  "${WINDRIVER_TOOL_ROOT}")

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
