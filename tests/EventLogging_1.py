#!/usr/bin/env python

#
# This tests the EventLogging mechanism; this is very similar to the testLog.py
# file in pex_logging, but uses EventLog instead of ScreenLog.  
# The events that are  sent are retrieved.
#


#import lsst.tests as tests
import os
import lsst.pex.logging as log
import lsst.ctrl.events as events
from lsst.daf.base import PropertySet
from socket import gethostname

if __name__ == "__main__":

    host = "lsst8.ncsa.uiuc.edu"
    topic = events.EventLog.LOGGING_TOPIC
    runid = "%s_%d" % (gethostname(), os.getpid())
    eventSystem = events.EventSystem.getDefaultEventSystem()
    eventSystem.createTransmitter(host,topic)
    eventSystem.createReceiver(host,topic, "RUNID = '%s'" % runid)

# test a simple message to the default log
    dlog = log.Log_getDefaultLog()
    dlog.log(log.Log.WARN, "this is a warning")

#    // now let's create our own root log
    logger = events.EventLog(runid, 665)

#    // test creation of child log
    tlog = log.Log(logger, "test")
    tlog.log(log.Log.INFO, "I like your hat")

    val = eventSystem.receiveEvent(topic, 100)
    assert val != None

#    // test threshold filtering
    tlog.setThreshold(log.Log.WARN)
    tlog.log(log.Log.INFO, "I like your gloves") #  // shouldn't see this 
    print "threshold is " , tlog.getThreshold()
    val = eventSystem.receiveEvent(topic, 100)
    assert val == None

#    // test the persistance of threshold levels
    tlog = log.Log(logger, "test")
    tlog.log(log.Log.INFO, "I like your shoes") #   // shouldn't see this 
    val = eventSystem.receiveEvent(topic, 100);
    assert val == None

    tlog.setThreshold(log.Log.DEBUG)
    tlog.log(log.Log.INFO, "I said, I like your shoes")
    val = eventSystem.receiveEvent(topic, 100);
    assert val != None

#    // test descendent log and ancestor's control of threshold
    tgclog = log.Log(tlog, "grand.child")   #   // name is now "test.grand.child"
    tgclog.log(log.Log.INFO, "Let's play")
    val = eventSystem.receiveEvent(topic, 100)
    assert val != None

    tlog.setThreshold(log.Log.FATAL)
    tgclog.log(log.Log.INFO, "You go first")

    val = eventSystem.receiveEvent(topic, 100);
    assert val == None

#    // test streaming
    log.LogRec(tgclog, log.Log.FATAL) << "help: I've fallen" << log.Prop("NODE", 5) << "& I can't get up" << log.endr;

    val = eventSystem.receiveEvent(topic, 100);
    assert val != None

    tmp = log.Prop("NODE",5)
    log.LogRec(tgclog, log.Log.FATAL) << "help: I've fallen" << tmp << "& I can't get up" << log.endr;

    val = eventSystem.receiveEvent(topic, 100);
    assert val != None
    print val.getPropertySet().toString()

#    // test flushing on delete
    log.LogRec(tgclog, log.Log.FATAL) << "never mind"
    tgclog = None

