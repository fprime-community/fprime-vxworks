The purpose of these \*.ini files is to serve as an example. Projects are expected to create their own .ini file in their project's repository and update their project's `settings.ini` file to point to their .ini file.

The .ini file is an environment file used when linking your F Prime software with VxWorks. There are at least three important variables:

`WIND_HOME`: This points to the VxWorks' installation directory.  
`WINDRIVER_COMPILER_ROOT`: This points to the VxWorks' directory where the bin directory lives. This path should be used by your toolchain file.  
`VSB_HOME`: This points to the directory of your VxWorks' source build (VSB). This path should be used by your toolchain file.
