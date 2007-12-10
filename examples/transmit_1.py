import lsst.events as events
import lsst.mwi.data as datap

if __name__ == "__main__":
    x = events.EventTransmitter("lsst8.ncsa.uiuc.edu", "test_logging")

    root = datap.SupportFactory.createPropertyNode("root");
    date = datap.DataProperty("DATE","2007-07-01T14:28:32.546012")
    pid = datap.DataProperty("PID",200)
    host = datap.DataProperty("HOST","fester.ncsa.uiuc.edu")
    ip = datap.DataProperty("IP","141.142.220.44")
    event = datap.DataProperty("EVNT","test")
    misc1 = datap.DataProperty("misc1","data 1")
    misc2 = datap.DataProperty("misc2","data 2")
    flo = datap.DataProperty("float_value", 3.14)

    root.addProperty(date);
    root.addProperty(pid);
    root.addProperty(host);
    root.addProperty(ip);
    root.addProperty(event);
    root.addProperty(misc1);
    root.addProperty(misc2);
    root.addProperty(flo);
    x.publish("log", root)
