"""demonstrate simple use of the Log facility."""
"""
testLog

Run with:
   python testLog
"""

import lsst.mwi.tests as tests
import lsst.mwi.utils as mwiu
import lsst.mwi.logging as log
import lsst.events as events
from lsst.mwi.data import DataProperty

if __name__ == "__main__":

    system = events.EventSystem().getDefaultEventSystem()
    system.createTransmitter("lsst8.ncsa.uiuc.edu","LSSTLogging")

# test a simple message to the default log
    dlog = log.Log_getDefaultLog()
    dlog.log(log.Log.WARN, "this is a warning")

#    // now let's create our own root log
    logger = events.EventLog("myRun", 123)
#    logger = log.ScreenLog(1)

#    // test creation of child log
    tlog = log.Log(logger, "test")
    tlog.log(log.Log.INFO, "I like your hat")
    print "message sent"
#    // test threshold filtering
    tlog.setThreshold(log.Log.WARN)
    tlog.log(log.Log.INFO, "I like your gloves") #  // shouldn't see this 
    print "threshold is " , tlog.getThreshold()

#    // test the persistance of threshold levels
    tlog = log.Log(logger, "test")
    tlog.log(log.Log.INFO, "I like your shoes") #   // shouldn't see this 
    tlog.setThreshold(log.Log.DEBUG)
    tlog.log(log.Log.INFO, "I said, I like your shoes")

#    // test descendent log and ancestor's control of threshold
    tgclog = log.Log(tlog, "grand.child")   #   // name is now "test.grand.child"
    tgclog.log(log.Log.INFO, "Let's play")
    tlog.setThreshold(log.Log.FATAL)
    tgclog.log(log.Log.INFO, "You go first")
    print "message sent"

#    // test streaming
    log.LogRec(tgclog, log.Log.FATAL) << "help: I've fallen" << DataProperty("NODE", 5) << "& I can't get up" << log.endr;
    print "message sent"
    tmp = DataProperty("NODE",5)
    log.LogRec(tgclog, log.Log.FATAL) << "help: I've fallen" << tmp << "& I can't get up" << log.endr;
    print "message sent"

#    // test flushing on delete
    log.LogRec(tgclog, log.Log.FATAL) << "never mind"
    tgclog = None
    print "message sent"

