#!/usr/bin/env python
import lsst.mwi.data as datap
import lsst.events as events
import lsst.mwi.logging as logging
import time
import sys

def printValue(log, info, dp):
    logRec = logging.LogRec(log, info)
    pValue(logRec,  dp)
    logRec << logging.endr;

def pValue(logRec, dp):
    if (dp.isNode() == True):
            kids = dp.getChildren()
            for i in kids:
                pValue(logRec, i)
    else:
        logRec << dp.get()

if __name__ == "__main__":
    print "starting...\n"
    host = sys.argv[1]
    topic = sys.argv[2]
    print host+"->"+topic
    x = events.EventReceiver(host, topic)

    logger = logging.ScreenLog(1)
    tlog = logging.Log(logger, "test")

    print "waiting on receive...\n"
    while (True):
        val = x.receive()
        if val.get() != None:
            printValue(tlog, logging.Log.INFO, val)

