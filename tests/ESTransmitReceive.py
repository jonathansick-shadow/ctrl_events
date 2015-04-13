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

class EventSystemReceiveTestCase(unittest.TestCase):
    """Test receiving via EventSystem"""

    def sendEvent(self, topicName):
        root = base.PropertySet()
        root.set("DATE","2007-07-01T14:28:32.546012")
        root.setInt("PID",200)
        root.set("HOST","lsstcorp.org")
        root.set("IP","1.2.3.4")
        root.set("EVNT","test")
        root.set("misc1","data 1")
        root.set("misc2","data 2")
        root.set("value", 3.14)
        
        eventSystem = events.EventSystem.getDefaultEventSystem()
        event = events.Event("runid_es4", root)
        eventSystem.publishEvent(topicName, event)

    @unittest.skipUnless(TestEnvironment().validTestDomain(), "not within valid domain")
    def testEventSystemReceive(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()
        thisHost = platform.node()

        topic = "test_events_%s_%d" % (thisHost, os.getpid())
    
        eventSystem = events.EventSystem.getDefaultEventSystem()
        eventSystem.createTransmitter(broker, topic)
        eventSystem.createReceiver(broker, topic)

        #
        # send a test event, and wait to receive it
        #
        self.sendEvent(topic)

        val = eventSystem.receiveEvent(topic)
        self.assertNotEqual(val, None)

        cpNames = val.getCustomPropertyNames()
        names = ["DATE", "PID", "HOST", "IP", "EVNT", "misc1", "misc2", "value"]

        self.assertEqual(len(cpNames), len(names))
        for x in names:
            self.assertTrue(x in cpNames)

        ps = val.getCustomPropertySet()

        self.assertEqual(ps.nameCount(), len(names))
        self.assertEqual(ps.get("DATE"), "2007-07-01T14:28:32.546012")
        self.assertEqual(ps.get("PID"), 200)
        self.assertEqual(ps.get("HOST"), "lsstcorp.org")
        self.assertEqual(ps.get("IP"), "1.2.3.4")
        self.assertEqual(ps.get("EVNT"), "test")
        self.assertEqual(ps.get("misc1"), "data 1")
        self.assertEqual(ps.get("misc2"), "data 2")
        self.assertEqual(ps.get("value"), 3.14)

        #
        # wait a short time to receive an event.  none was sent, so we should
        # time out and confirm that we didn't get anything
        #
        val = eventSystem.receiveEvent(topic, 1000)
        self.assertEqual(val, None)

def suite():
    """Returns a suite containing all the tests cases in this module."""
    tests.init()
    suites = []
    suites += unittest.makeSuite(EventSystemReceiveTestCase)
    suites += unittest.makeSuite(tests.MemoryTestCase)
    return unittest.TestSuite(suites)

def run(shouldExit=False):
    """Run the tests."""
    tests.run(suite(), shouldExit)

if __name__ == "__main__":
    run(True)
