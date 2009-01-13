import lsst.ctrl.events as events
import lsst.daf.base as base

if __name__ == "__main__":
    x = events.EventTransmitter("lsst8.ncsa.uiuc.edu", "test_logging")

    root = base.PropertySet()
    root.add("DATE","2007-07-01T14:28:32.546012")
    root.addInt("PID",200)
    root.add("HOST","lsst8.ncsa.uiuc.edu")
    root.add("IP","141.142.220.44")
    root.add("EVNT","test")
    root.add("misc1","data 1")
    root.add("misc2","data 2")
    root.addFloat("float_value", 3.14)

    x.publish(root)
