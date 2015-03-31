#!/usr/bin/env python

# 
# LSST Data Management System
#
# Copyright 2008-2014  AURA/LSST.
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
# see <https://www.lsstcorp.org/LegalNotices/>.
#

import os
import platform
import unittest
import lsst.ctrl.events as events
import lsst.daf.base as base
import lsst.utils.tests as tests
from testEnvironment import TestEnvironment

class MultiTopicSendTestCase(unittest.TestCase):
    """Send a multitopic event"""

    def sendEvent(self, broker, topic):
        trans = events.EventTransmitter(broker, topic)
        
        root = base.PropertySet()
        root.set("DATE","2007-07-01T14:28:32.546012")
        root.setInt("PID",200)
        root.set("HOST","lsstcorp.org")
        root.set("IP","1.2.3.4")
        root.set("EVNT","test")
        root.set("misc1","data 1")
        root.set("misc2","data 2")
        root.set("value", 3.14)
    
        event = events.Event("runid_test6",root)
        trans.publishEvent(event)

    def testMultiTopicSend(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()
        thisHost = platform.node()

        pid = os.getpid()
    
        host_pid = "%s_%d" % (thisHost, pid)
    
        topic1 = "test_events_6_%s" % host_pid
        topic2 = "test_events_6a_%s" % host_pid
       
        y1 = events.EventReceiver(broker, topic1)
      
        y2 = events.EventReceiver(broker, topic2)
    
        #
        # send a test event on both topics at once, and have each receiver wait to
        # receive it
        #
        self.sendEvent(broker, topic1+","+topic2)
    
        val = y1.receiveEvent()
        self.assertNotEqual(val, None)
        ps = val.getPropertySet()
    
        val = y2.receiveEvent()
        self.assertNotEqual(val, None)
        ps = val.getPropertySet()

def suite():
    """Returns a suite containing all the tests cases in this module."""
    tests.init()
    suites = []
    suites += unittest.makeSuite(MultiTopicSendTestCase)
    suites += unittest.makeSuite(tests.MemoryTestCase)
    return unittest.TestSuite(suites)

def run(shouldExit=False):
    """Run the tests."""
    tests.run(suite(), shouldExit)

if __name__ == "__main__":
    run(True)
