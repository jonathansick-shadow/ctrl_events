import lsst.ctrl.events as events
import time
import lsst.daf.base as datap
import lsst.pex.policy as policy

if __name__ == "__main__":
    print "starting...\n"
    p = policy.Policy.createPolicy("examples/policies/activemq_policy.paf", 1)

    x = events.EventReceiver(p)
    print "waiting on receive...\n"
    val = x.receive()
    print "finished!\n"
    print val.get()
    if val.get() != None:
        print val.toString("=", 1)
