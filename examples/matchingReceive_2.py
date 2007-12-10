
import lsst.mwi.data as datap
import lsst.events as events
import time

if __name__ == "__main__":
    print "starting...\n"
    x = events.EventReceiver("lsst8.ncsa.uiuc.edu", "test")

    print "waiting on matchingReceive... for PID == 200\n"

    val = x.matchingReceive("PID", 200, 10000)
    print "finished!\n"
    print val.get()
    if val.get() != None:
        print val.toString("=", 1)

    print "waiting on matchingReceive... for PID == 300\n"
    val = x.matchingReceive("PID", 300, 10000)
    print "finished!\n"
    print val.get()
    if val.get() != None:
        print val.toString("=", 1)
   

    print "waiting on receive...\n"
    val = x.receive(10000)
    print "finished!\n"
    print val.get()
    if val.get() != None:
        print val.toString("=", 1)

    print "waiting on receive...\n"
    val = x.receive(10000)
    print "finished!\n"
    print val.get()
    if val.get() != None:
        print val.toString("=", 1)

    print "waiting on receive...\n"
    val = x.receive(10000)
    print "finished!\n"
    print val.get()
    if val.get() != None:
        print val.toString("=", 1)
