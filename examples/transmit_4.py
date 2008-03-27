import lsst.events as events
import lsst.daf.data as datap
import lsst.pex.policy as policy

if __name__ == "__main__":
    p = policy.Policy.createPolicy("examples/policies/localsockets_policy.paf", 1)
    x = events.EventTransmitter(p)

    root = datap.SupportFactory.createPropertyNode("root")
    pid = datap.DataProperty("PID",200)
    text = datap.DataProperty("text", "hello, world")
    flo = datap.DataProperty("flo", 3.14)

    root.addProperty(pid)
    root.addProperty(text)
    root.addProperty(flo)
    x.publish("log", root)
