# Set system name
set(CMAKE_SYSTEM_NAME VxWorks)
set(CMAKE_SYSTEM_VERSION 6.9)
set(CMAKE_SYSTEM_PROCESSOR PPC)

set(WIND_COMP "$ENV{WIND_COMP}")

# specify the cross compiler
set(CMAKE_C_COMPILER "${WIND_COMP}/ccppc")
set(CMAKE_CXX_COMPILER "${WIND_COMP}/c++ppc")
set(CMAKE_AR "${WIND_COMP}/arppc")
set(CMAKE_RANLIB "${WIND_COMP}/ranlibppc")
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


set(COMPILER_COMMON_FLAGS
    "-DTOOL_FAMILY=gnu \
    -DTOOL=gnu \
    -Wall \
    -Wextra \
    -fno-builtin \
    -fstrength-reduce \
    -Wno-unused-parameter \
    -Wno-long-long \
    -Werror "
    )
    
set(CMAKE_C_FLAGS
    "${COMPILER_COMMON_FLAGS} \
    -std=c99 \
    -fno-cond-mismatch -pedantic \
    -Werror-implicit-function-declaration \
    -Wstrict-prototypes"
    )  

set(CMAKE_CXX_FLAGS
    "${COMPILER_COMMON_FLAGS} \
    -fcheck-new \
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
