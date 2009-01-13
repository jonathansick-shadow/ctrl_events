import lsst.ctrl.events as events
import lsst.daf.base as base
import lsst.pex.policy as policy

if __name__ == "__main__":
    p = policy.Policy.createPolicy("policies/localsockets_policy.paf", 1)
    x = events.EventTransmitter(p)

    root = base.PropertySet()
    root.addInt("PID",200)
    root.add("text", "hello, world")
    root.addFloat("flo", 3.14)

    x.publish(root)
