#!/usr/bin/env python
#
# logwatcher.py - watches for logging events, and prints them
#
#
import lsst.daf.base as datap
import lsst.ctrl.events as events
import time
import sys

if __name__ == "__main__":
    host = sys.argv[1]
    runid = sys.argv[2]
    print "listening on host "+host
    x = events.EventReceiver(host, events.EventLog.LOGGING_TOPIC, "RUNID = '%s'" % runid )
    while (True): 
        print "waiting on logging event...\n"
        event = x.receiveEvent()
        val = event.getPropertySet()
        if val != None:
            print val.toString()
