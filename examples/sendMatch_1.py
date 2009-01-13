import lsst.ctrl.events as events
import lsst.daf.base as base

if __name__ == "__main__":
    x = events.EventTransmitter("lsst8.ncsa.uiuc.edu", "test")

    root0 = base.PropertySet()
    root0.addInt("PID",400)
    root0.add("EVNT","sent first; received third")
    x.publish(root0)

    root1 = base.PropertySet()
    root1.addInt("PID",300)
    root1.add("EVNT","sent second; received second")
    x.publish(root1)

    root2 = base.PropertySet()
    root2.addInt("PID",500)
    root2.add("EVNT","send third; received fourth")
    x.publish(root2)

    root3 = base.PropertySet()
    root3.addInt("PID",200)
    root3.add("EVNT","sent fourth: received first")
    x.publish(root3)
