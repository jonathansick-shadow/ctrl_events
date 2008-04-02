import lsst.daf.base as datap
import lsst.ctrl.events as events

if __name__ == "__main__":

    system = events.EventSystem().getDefaultEventSystem()
    system.createTransmitter("lsst8.ncsa.uiuc.edu", "test1")
    system.createTransmitter("lsst8.ncsa.uiuc.edu", "test2")
    system.createTransmitter("lsst8.ncsa.uiuc.edu", "test3")

    newSystem = events.EventSystem().getDefaultEventSystem()

    root = datap.DataProperty.createPropertyNode("root");
    date = datap.DataProperty("DATE","2007-07-01T14:28:32.546012")
    pid = datap.DataProperty("PID",200)
    host = datap.DataProperty("HOST","fester.ncsa.uiuc.edu")
    ip = datap.DataProperty("IP","141.142.220.44")
    event = datap.DataProperty("EVNT","test")
    misc1 = datap.DataProperty("misc1","data 1")
    misc2 = datap.DataProperty("misc2","data 2")
    flo = datap.DataProperty("float_value", 3.14)

    root.addProperty(date)
    root.addProperty(pid)
    root.addProperty(host)
    root.addProperty(ip)
    root.addProperty(event)
    root.addProperty(misc1)
    root.addProperty(misc2)
    root.addProperty(flo)

    newSystem.publish("test1", root, "type1")
    newSystem.publish("test2", root, "type2")
    newSystem.publish("test3", root, "type3")
