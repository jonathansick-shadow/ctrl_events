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

class CombinedEventTestCase(unittest.TestCase):
    """test sending one message two multiple topics with one publish"""

    def sendEvent(self, topicName):
        
        root = base.PropertySet()
        root.set("DATE", "2007-07-01T14:28:32.546012")
        root.setInt("PID", 200)
        root.set("HOST", "lsstcorp.org")
        root.set("IP", "1.2.3.4")
        root.set("EVNT", "test")
        root.set("misc1", "data 1")
        root.set("misc2", "data 2")
        root.set("value", 3.14)
        
        eventSystem = events.EventSystem.getDefaultEventSystem()
        event = events.Event("runid_es6", root)
        eventSystem.publishEvent(topicName, event)

    @unittest.skipUnless(TestEnvironment().validTestDomain(), "not within valid domain")
    def testCombinedEvent(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()

        topic1 = "test_events_3_%s_%d" % (platform.node(), os.getpid())
        topic2 = "test_events_3a_%s_%d" % (platform.node(), os.getpid())
        combinedTopic = topic1 + "," + topic2
        eventSystem = events.EventSystem.getDefaultEventSystem()
        eventSystem.createReceiver(broker, topic1)
        eventSystem.createReceiver(broker, topic2)
        eventSystem.createTransmitter(broker, combinedTopic)
    
        #
        # send a test event on both topics at once, and have each receiver wait to
        # receive it
        #
        self.sendEvent(combinedTopic)
    
        val = eventSystem.receiveEvent(topic1)
        self.assertIsNotNone(val)
        ps = val.getPropertySet()
        print ps.toString()
    
        val = eventSystem.receiveEvent(topic2)
        self.assertIsNotNone(val)
        ps = val.getPropertySet()
        print ps.toString()

def suite():
    """Returns a suite containing all the tests cases in this module."""
    tests.init()
    suites = []
    suites += unittest.makeSuite(CombinedEventTestCase)
    suites += unittest.makeSuite(tests.MemoryTestCase)
    return unittest.TestSuite(suites)

def run(shouldExit=False):
    """Run the tests."""
    tests.run(suite(), shouldExit)

if __name__ == "__main__":
    run(True)
