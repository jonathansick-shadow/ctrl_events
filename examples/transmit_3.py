import lsst.events as events
import lsst.daf.data as datap
import lsst.pex.policy as policy

if __name__ == "__main__":
    p = policy.Policy.createPolicy("examples/policies/activemq_policy.paf", 1)
    x = events.EventTransmitter(p)

    root = datap.SupportFactory.createPropertyNode("root")
    pid = datap.DataProperty("PID",200)
    text = datap.DataProperty("text", "hello, world")
    flo = datap.DataProperty("flo", 3.14)

    root.addProperty(pid)
    root.addProperty(text)
    root.addProperty(flo)

    subroot1 = datap.SupportFactory.createPropertyNode("subroot1")
    pid2 = datap.DataProperty("PID2",300)
    text2 = datap.DataProperty("text2", "hi there")
    flo2 = datap.DataProperty("flo2", 2.11)

    subroot1.addProperty(pid2)
    subroot1.addProperty(text2)
    subroot1.addProperty(flo2)

    subsubroot = datap.SupportFactory.createPropertyNode("subsubroot")
    subpid = datap.DataProperty("sub PID",3)
    subtext = datap.DataProperty("sub text", "woof")
    subflo = datap.DataProperty("sub flo", 7.08)

    subsubroot.addProperty(subpid)
    subsubroot.addProperty(subtext)
    subsubroot.addProperty(subflo)
    subroot1.addProperty(subsubroot)
    root.addProperty(subroot1)

    subroot2 = datap.SupportFactory.createPropertyNode("subroot2")
    pid3 = datap.DataProperty("PID3",400)
    text3 = datap.DataProperty("text3", "moo cow")
    flo3 = datap.DataProperty("flo3", 1.87)

    subroot2.addProperty(pid2)
    subroot2.addProperty(text2)
    subroot2.addProperty(flo2)

    root.addProperty(subroot2)

    print root.toString("-",1)
    x.publish("log", root)
