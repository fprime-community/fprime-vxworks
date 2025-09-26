module VxWorksSvc {
    @ VxWorks WatchDog Timer
    passive component VxWatchDogTimer {

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ implement tick interface
    import Drv.Tick

    }
}
