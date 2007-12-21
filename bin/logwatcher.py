#!/usr/bin/env python
import lsst.mwi.data as datap
import lsst.events as events
import time

if __name__ == "__main__":
    print "starting...\n"
    x = events.EventReceiver("lsst8.ncsa.uiuc.edu", "LSSTLogging")
    while (True): 
        print "waiting on logging event...\n"
        val = x.receive()
        if val.get() != None:
            print val.toString("=", 1)
