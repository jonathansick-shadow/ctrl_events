import time
import lsst.ctrl.events as events
import lsst.daf.base as base

if __name__ == "__main__":
    x = events.EventTransmitter("lsst8.ncsa.uiuc.edu", "test")

    root0 = base.PropertySet()
    root0.addInt("PID",100)
    root0.add("EVNT","sent first; received third")
    x.publish(root0)

    time.sleep(6);

    root1 = base.PropertySet()
    root1.addInt("PID",400)
    root1.add("EVNT","sent second; received fourth")
    x.publish(root1)

    root2 = base.PropertySet()
    root2.addInt("PID",200)
    root2.add("EVNT","sent third; received first")
    x.publish(root2)

    root3 = base.PropertySet()
    root3.addInt("PID",500)
    root3.add("EVNT","sent fourth: received fifth")
    x.publish(root3)

    root4 = base.PropertySet()
    root4.addInt("PID",300)
    root4.add("EVNT","send fifth; received second")
    x.publish(root4)

