import lsst.ctrl.events as events

if __name__ == "__main__":

    system = events.EventSystem.getDefaultEventSystem()
    system.createReceiver("lsst8.ncsa.uiuc.edu", "test1")
    system.createReceiver("lsst8.ncsa.uiuc.edu", "test2")
    system.createReceiver("lsst8.ncsa.uiuc.edu", "test3")

    dp1 = system.receive("test1")
    print dp1.toString()

    dp2 = system.receive("test2")
    print dp2.toString()

    dp3 = system.receive("test3")
    print dp3.toString()

