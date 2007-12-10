import lsst.mwi.data as datap
import lsst.events as events
import time

if __name__ == "__main__":
    print "starting...\n"
    x = events.EventReceiver("lsst8.ncsa.uiuc.edu", "LSSTLogging")
    print "waiting on receive...\n"
    for i in range(8): 
        val = x.receive(8000)
        print "finished!\n"
        if val.get() != None:
            print val.toString("=", 1)
