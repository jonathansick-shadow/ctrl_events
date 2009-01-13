import lsst.ctrl.events as events
import time

if __name__ == "__main__":
    print "starting...\n"
    x = events.EventReceiver("lsst8.ncsa.uiuc.edu", "test")

    print "waiting on receive...\n"
    val = x.receive()
    print "waiting on matchingReceive... for PID == 200\n"

    val = x.matchingReceive("PID",200)
    print "finished!\n"
    if val != None:
        print val.toString()

    print "waiting on matchingReceive... for PID == 300\n"
    val = x.matchingReceive("PID",300)
    print "finished!\n"
    if val != None:
        print val.toString()
   

    print "waiting on receive...\n"
    val = x.receive()
    print "finished!\n"
    if val != None:
        print val.toString()

