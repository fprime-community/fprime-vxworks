restrict_platforms(vxworks)

set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/VxWatchDogTimer.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/VxWatchDogTimer.cpp"
)

register_fprime_module()