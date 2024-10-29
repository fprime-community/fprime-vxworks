####
# VxWorks-common.cmake:
#
# Platform setup for all VxWorks based targets. This can be used for
# Sphinx, Sabertooth and other hosts.
####

choose_fprime_implementation(Os/Console Os/Console/VxWorks)
choose_fprime_implementation(Os/Cpu Os/Cpu/Stub)
choose_fprime_implementation(Os/File Os/File/Stub)
choose_fprime_implementation(Os/Memory Os/Memory/Stub)
choose_fprime_implementation(Os/Mutex Os/Mutex/VxWorks)
choose_fprime_implementation(Os/Queue Os/Generic/PriorityQueue)
choose_fprime_implementation(Os/RawTime Os/RawTime/Stub)
choose_fprime_implementation(Os/Task Os/Task/VxWorks)
