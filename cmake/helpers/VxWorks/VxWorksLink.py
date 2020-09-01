#!/usr/bin/python

# arguments to script
#
# 1 = binary target
# 2 on = libraries/objects to link

import sys
import os
import os.path
import subprocess

# skip the -o
binary = sys.argv[2] # $@ in make file
libs = " ".join(sys.argv[3:]) # $^ + other libraries

# Environment variables below are defined in compiler config files and passed by make

prelink_line =  os.environ["LINK_BIN_PRE"] + " " + \
                os.environ["LINK_BIN_PRE_FLAGS"] + " " + \
                os.environ["LINK_BIN_PRE_TO"] + " " + \
                binary + ".pre_ctdt " + libs + " " + \
                os.environ["LINK_BIN_PRE_EXT_LIB"]

                

dump_line =     os.environ["DUMP"] + " " + binary + \
                ".pre_ctdt | " + os.environ["MUNCH"] + \
                " > " + os.path.dirname(binary) + os.sep + "ctdt.c"


munch_line =    os.environ["CC_MUNCH"] + " " + \
                os.environ["CFLAGS"] + " " +  \
                os.path.dirname(binary) + os.sep + "ctdt.c" +  " " + \
                os.environ["COMPILE_ONLY"] + " " + \
                os.environ["COMPILE_TO"] + " " + \
                os.path.dirname(binary) + os.sep + "ctdt.o"


link_line =     os.environ["LINK_BIN_POST"] + " " + \
                os.environ["LINK_BIN_POST_FLAGS"] + " " + \
                os.environ["LINK_BIN_POST_TO"] + " " + \
                binary + " " + binary +".pre_ctdt " + \
                os.path.dirname(binary) + os.sep + "ctdt.o"
                
                

missing_symbols_line = os.environ["SYMBOL_CHECK"] + " " + \
                binary + " " + os.environ["SYMBOL_CHECK_FILE"] + \
                " | " + os.environ["DEMANGLE"]
                
#print "Missing symbols line: %s"%missing_symbols_line

# invoke each line 
print("Pre-linking binary...")
print( prelink_line)
ret = subprocess.call(prelink_line,shell="true")
if (ret != 0):
    sys.exit(-1)

print( "Dumping binary...")
print( dump_line)
subprocess.call(dump_line,shell="true")
if (ret != 0):
    sys.exit(-1)
# 
print( "Munching binary...")
print(munch_line)
subprocess.call(munch_line,shell="true")
if (ret != 0):
    sys.exit(-1)
# 
print( "Linking binary...")
print( link_line)
subprocess.call(link_line,shell="true")
if (ret != 0):
    sys.exit(-1)

print( "Checking for missing symbols...")
print( missing_symbols_line)
subprocess.call(missing_symbols_line,shell="true")
if (ret != 0):
    sys.exit(-1)
