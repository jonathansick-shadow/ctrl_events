import lsst.mwi.data as datap
import lsst.events as events
import time

if __name__ == "__main__":
    print "starting...\n"
    x = events.EventReceiver("lsst8.ncsa.uiuc.edu", "logging")
    print x
    print "waiting on receive...\n"
    val = x.receive()
    print "finished!\n"
    print val.get()
    if val.get() != None:
        print val.toString("=", 1)
