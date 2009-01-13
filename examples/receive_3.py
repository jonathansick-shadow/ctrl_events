import lsst.ctrl.events as events
import time
import lsst.pex.policy as policy

if __name__ == "__main__":
    print "starting...\n"
    p = policy.Policy.createPolicy("policies/activemq_policy.paf", 1)

    x = events.EventReceiver(p)
    print "waiting on receive...\n"
    val = x.receive()
    print "finished!\n"
    if val != None:
        print val.toString()
    else:
        print "should have received something, but didn't"
