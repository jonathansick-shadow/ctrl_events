#!/usr/bin/env python

#
# This tests the EventLogging mechanism; this is very similar to the testLog.py
# file in mwi, but uses EventLog instead of ScreenLog.  The events that are
# sent are retrieved.
#


#import lsst.mwi.tests as tests
#import lsst.mwi.utils as mwiu
import lsst.mwi.logging as log
import lsst.events as events
from lsst.mwi.data import DataProperty

if __name__ == "__main__":

    host = "lsst8.ncsa.uiuc.edu"
    topic = "LSSTLogging"
    eventSystem = events.EventSystem.getDefaultEventSystem()
    eventSystem.createTransmitter(host,topic)
    eventSystem.createReceiver(host,topic)

# test a simple message to the default log
    dlog = log.Log_getDefaultLog()
    dlog.log(log.Log.WARN, "this is a warning")

#    // now let's create our own root log
    logger = events.EventLog("myRunId", 665)

#    // test creation of child log
    tlog = log.Log(logger, "test")
    tlog.log(log.Log.INFO, "I like your hat")

    val = eventSystem.receive(topic, 100);
    assert val.get != None

#    // test threshold filtering
    tlog.setThreshold(log.Log.WARN)
    tlog.log(log.Log.INFO, "I like your gloves") #  // shouldn't see this 
    print "threshold is " , tlog.getThreshold()
    val = eventSystem.receive(topic, 100);
    assert val.get() == None

#    // test the persistance of threshold levels
    tlog = log.Log(logger, "test")
    tlog.log(log.Log.INFO, "I like your shoes") #   // shouldn't see this 
    val = eventSystem.receive(topic, 100);
    assert val.get() == None

    tlog.setThreshold(log.Log.DEBUG)
    tlog.log(log.Log.INFO, "I said, I like your shoes")
    val = eventSystem.receive(topic, 100);
    assert val.get() != None

#    // test descendent log and ancestor's control of threshold
    tgclog = log.Log(tlog, "grand.child")   #   // name is now "test.grand.child"
    tgclog.log(log.Log.INFO, "Let's play")
    val = eventSystem.receive(topic, 100);
    assert val.get() != None
    tlog.setThreshold(log.Log.FATAL)
    tgclog.log(log.Log.INFO, "You go first")

    val = eventSystem.receive(topic, 100);
    assert val.get() == None

#    // test streaming
    log.LogRec(tgclog, log.Log.FATAL) << "help: I've fallen" << DataProperty("NODE", 5) << "& I can't get up" << log.endr;

    val = eventSystem.receive(topic, 100);
    assert val.get() != None
    tmp = DataProperty("NODE",5)
    log.LogRec(tgclog, log.Log.FATAL) << "help: I've fallen" << tmp << "& I can't get up" << log.endr;

    val = eventSystem.receive(topic, 100);
    assert val.get() != None

#    // test flushing on delete
    log.LogRec(tgclog, log.Log.FATAL) << "never mind"
    tgclog = None

