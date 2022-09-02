# Use VxWorks common file
#include_directories(/opt/WindRiver/compilers/gnu-8.3.0.2/x86_64-linux2/lib/gcc/powerpc-linux-gnuspe/8.3.0/include/)
include_directories("/opt/WindRiver/vxworks-7/pkgs_v2/os/arch/ppc-2.0.4.0/kernel/base/h")
include_directories("/opt/WindRiver/vxworks-7/pkgs_v2/os/arch/ppc-2.0.4.0/kernel/60x/h")
include_directories("/opt/WindRiver/vxworks-7/pkgs_v2/os/lang_lib/tool/intrinsics_gnu-1.0.2.2/h")
include("${CMAKE_CURRENT_LIST_DIR}/VxWorks-common.cmake")

include_directories(SYSTEM "${CMAKE_CURRENT_LIST_DIR}/types")