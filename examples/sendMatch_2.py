import time
import lsst.events as events
import lsst.daf.data as datap

if __name__ == "__main__":
    x = events.EventTransmitter("lsst8.ncsa.uiuc.edu", "test")

    root0 = datap.SupportFactory.createPropertyNode("root");
    pid0 = datap.DataProperty("PID",100)
    event0 = datap.DataProperty("EVNT","sent first; received third")
    root0.addProperty(pid0);
    root0.addProperty(event0);
    x.publish("log", root0)

    time.sleep(6);

    root1 = datap.SupportFactory.createPropertyNode("root");
    pid1 = datap.DataProperty("PID",400)
    event1 = datap.DataProperty("EVNT","sent second; received fourth")
    root1.addProperty(pid1);
    root1.addProperty(event1);
    x.publish("log", root1)

    root2 = datap.SupportFactory.createPropertyNode("root");
    pid2 = datap.DataProperty("PID",200)
    event2 = datap.DataProperty("EVNT","sent third; received first")
    root2.addProperty(pid2);
    root2.addProperty(event2);
    x.publish("log", root2)

    root3 = datap.SupportFactory.createPropertyNode("root");
    pid3 = datap.DataProperty("PID",500)
    event3 = datap.DataProperty("EVNT","sent fourth: received fifth")
    root3.addProperty(pid3);
    root3.addProperty(event3);
    x.publish("log", root3)

    root4 = datap.SupportFactory.createPropertyNode("root");
    pid4 = datap.DataProperty("PID",300)
    event4 = datap.DataProperty("EVNT","send fifth; received second")
    root4.addProperty(pid4);
    root4.addProperty(event4);
    x.publish("log", root4)

