#!/usr/bin/env python

# 
# LSST Data Management System
# Copyright 2008, 2009, 2010 LSST Corporation.
# 
# This product includes software developed by the
# LSST Project (http://www.lsst.org/).
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the LSST License Statement and 
# the GNU General Public License along with this program.  If not, 
# see <http://www.lsstcorp.org/LegalNotices/>.
#


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

    host = "lsst8.ncsa.illinois.edu"

    # note that in this test, while the topic is globally named, the 
    # selector for the receiver is not.  The value for RUNID is included
    # in all logging messages sent, and the broker uses this to
    # deliver only messages sent with the RUNID to the receiver, since it
    # was specified here.  If the receiver did not include the selector
    # for this RUNID, this test would have a small chance of failing if
    # run at the same time as another test on another machine. Since the
    # selector is specified, it will only get the messages with that RUNID.

    topic = events.EventLog.LOGGING_TOPIC
    runid = "%s_%d" % (gethostname(), os.getpid())
    eventSystem = events.EventSystem.getDefaultEventSystem()
    eventSystem.createTransmitter(host,topic)
    eventSystem.createReceiver(host,topic, "RUNID = '%s'" % runid)

# test a simple message to the default log
#    dlog = log.Log_getDefaultLog()
#    dlog.log(log.Log.WARN, "this is a warning")

#    // now let's create our own root log
    logger = events.EventLog(runid, 665)

#    // test creation of child log
    tlog = log.Log(logger, "test")
    tlog.log(log.Log.INFO, "I like your hat")

    val = eventSystem.receiveEvent(topic, 1000)
    assert val != None

#    // test threshold filtering
    tlog.setThreshold(log.Log.WARN)
    tlog.log(log.Log.INFO, "I like your gloves") #  // shouldn't see this 
    print "threshold is " , tlog.getThreshold()
    val = eventSystem.receiveEvent(topic, 1000)
    assert val == None

#    // test the persistance of threshold levels
    tlog = log.Log(logger, "test")
    tlog.log(log.Log.INFO, "I like your shoes") #   // shouldn't see this 
    val = eventSystem.receiveEvent(topic, 1000)
    assert val == None

    tlog.setThreshold(log.Log.DEBUG)
    tlog.log(log.Log.INFO, "I said, I like your shoes")
    val = eventSystem.receiveEvent(topic, 1000)
    assert val != None

#    // test descendent log and ancestor's control of threshold
    tgclog = log.Log(tlog, "grand.child")   #   // name is now "test.grand.child"
    tgclog.log(log.Log.INFO, "Let's play")
    val = eventSystem.receiveEvent(topic, 1000)
    assert val != None

    tlog.setThreshold(log.Log.FATAL)
    tgclog.log(log.Log.INFO, "You go first")

    val = eventSystem.receiveEvent(topic, 1000)
    assert val == None

#    // test streaming
    log.LogRec(tgclog, log.Log.FATAL) << "help: I've fallen" << log.Prop("NODE", 5) << "& I can't get up" << log.endr

    val = eventSystem.receiveEvent(topic, 1000)
    assert val != None

    tmp = log.Prop("NODE",5)
    log.LogRec(tgclog, log.Log.FATAL) << "help: I've fallen" << tmp << "& I can't get up" << log.endr

    val = eventSystem.receiveEvent(topic, 1000)
    assert val != None
    print val.getPropertySet().toString()

#    // test flushing on delete
    log.LogRec(tgclog, log.Log.FATAL) << "never mind"
    tgclog = None

