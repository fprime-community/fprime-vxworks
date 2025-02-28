# Project Setup

## settings.ini
Make sure to add the following to your project's `settings.ini` file:

1. Add the path to `fprime-vxworks` to `library_locations`
2. Add to `default_cmake_options` the following where `FPRIME_PLATFORM` is assigned to your selected platform (`VxWorks` in this case)
   ```cmake
   VX_TARGET_TYPE=DKM
   FPRIME_PLATFORM=VxWorks
   ```
3. Define `WIND_CC_SYSROOT` in the `environment` section. This variable should point to your project's VxWorks Source Build (VSB).

> NOTE: `VX_TARGET_TYPE`, `FPRIME_PLATFORM`, and `WIND_CC_SYSROOT` are required definitions by this VxWorks toolchain. 

### Example settings.ini
This is an example for a project's `settings.ini` file that uses fprime-vxworks for POSIX. 
```
[fprime]
project_root: .
framework_path: ./lib/fprime
library_locations: ./lib/fprime-vxworks
config_directory: ./config
default_toolchain: vxworks

default_cmake_options:  FPRIME_ENABLE_FRAMEWORK_UTS=OFF
                        FPRIME_ENABLE_AUTOCODER_UTS=OFF
                        VX_TARGET_TYPE=DKM
                        FPRIME_PLATFORM=VxWorks

[environment]
WIND_CC_SYSROOT: $HOME/projects/vxworks/vxworks-7-bsp-generator/bsp/VxWorks7-22.09-BeagleBoneBlack-Source
```
## Configuration
Copy the configuration file [`config/VxWorksCfg.hpp`](config/VxWorksCfg.hpp) to your project's configuration directory.