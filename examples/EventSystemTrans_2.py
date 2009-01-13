import lsst.daf.base as base
import lsst.ctrl.events as events

if __name__ == "__main__":

    system = events.EventSystem().getDefaultEventSystem()
    system.createTransmitter("lsst8.ncsa.uiuc.edu", "test1")
    system.createTransmitter("lsst8.ncsa.uiuc.edu", "test2")
    system.createTransmitter("lsst8.ncsa.uiuc.edu", "test3")

    newSystem = events.EventSystem().getDefaultEventSystem()

    root = base.PropertySet()
    root.add("DATE","2007-07-01T14:28:32.546012")
    root.addInt("PID",200)
    root.add("HOST","lsst8.ncsa.uiuc.edu")
    root.add("IP","141.142.220.44")
    root.add("EVNT","test")
    root.add("misc1","data 1")
    root.add("misc2","data 2")
    root.addDouble("float_value", 3.14)

    newSystem.publish("test1", root)
    newSystem.publish("test2", root)
    newSystem.publish("test3", root)
