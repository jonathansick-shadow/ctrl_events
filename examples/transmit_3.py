import lsst.ctrl.events as events
import lsst.daf.base as base
import lsst.pex.policy as policy

if __name__ == "__main__":
    p = policy.Policy.createPolicy("policies/activemq_policy.paf", 1)
    x = events.EventTransmitter(p)

    root = base.PropertySet()
    root.addInt("PID",200)
    root.add("text", "hello, world")
    root.addFloat("flo", 3.14)

    root.addInt("PID2",300)
    root.add("text2", "hi there")
    root.addFloat("flo2", 2.11)

    root.addInt("sub PID",3)
    root.add("sub text", "woof")
    root.addFloat("sub flo", 7.08)


    root.addInt("PID3",400)
    root.add("text3", "moo cow")
    root.addFloat("flo3", 1.87)

    print root.toString()
    x.publish(root)
