#!/usr/bin/env python
import lsst.mwi.data as datap
import lsst.events as events
import time
import sys

if __name__ == "__main__":
    host = sys.argv[1]
    print "starting...\n"
    x = events.EventReceiver(host, "LSSTLogging")
    while (True): 
        print "waiting on logging event...\n"
        val = x.receive()
        if val.get() != None:
            print val.toString("=", 1)
