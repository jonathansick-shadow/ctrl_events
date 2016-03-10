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
import time
import unittest
import lsst.ctrl.events as events
from lsst.daf.base import PropertySet
import lsst.utils.tests as tests
from testEnvironment import TestEnvironment

class CombinedReceiveEventTestCase(unittest.TestCase):
    """Test receiving on one topic, when it is sent to two"""

    def sendEvent(self, broker, topic):
        trans = events.EventTransmitter(broker, topic)
        
        root = PropertySet()
        root.set("TOPIC",topic)
        root.set("myname","myname")
        root.set("STATUS", "my special status")
        root.set("RUNID","srptestrun")
        
        event = events.Event("srptestrun", root)
    
        # wait a short time so we can see the difference between the time 
        # the event is created and the time it is published
        time.sleep(2)
    
        # ok...now publish it
        trans.publishEvent(event)

    @unittest.skipUnless(TestEnvironment().validTestDomain(), "not within valid domain")
    def testCombinedReceive(self):
        # Test sending on two topics, and receiving bother on a third.
        #
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()
        thisHost = platform.node()

        pid = os.getpid()
        host_pid = "%s_%d" % (thisHost,pid)
    
        topicA = "test_events_9_%s.A" % host_pid
        topicB = "test_events_9_%s.B" % host_pid
        topicC = "test_events_9_%s.*" % host_pid
    
        recvA = events.EventReceiver(broker, topicA)
        recvB = events.EventReceiver(broker, topicB)
        recvC = events.EventReceiver(broker, topicC)
    
        # send a test event on topicA and topicB
        self.sendEvent(broker, topicA)
        self.sendEvent(broker, topicB)

        # receive event on topicA, and check to see it's the right one
        val = recvA.receiveEvent()
        self.assertIsNotNone(val)
        ps = val.getPropertySet()
        name = ps.get("TOPIC")
        self.assertEqual(name, topicA)

        # receiving no more messages on topicA
        val = recvA.receiveEvent(1)
        self.assertIsNone(val)

        # receive event on topicB, and check to see it's the right one
        val = recvB.receiveEvent()
        self.assertIsNotNone(val)
        ps = val.getPropertySet()
        name = ps.get("TOPIC")
        self.assertEqual(name, topicB)

        # receiving no more messages on topicB
        val = recvB.receiveEvent(1)
        self.assertIsNone(val)
    
        # receive event on topicC, and check to see it's the one sent to topicA
        val = recvC.receiveEvent()
        self.assertIsNotNone(val)
        ps = val.getPropertySet()
        name = ps.get("TOPIC")
        self.assertEqual(name, topicA)
    
        # receive event on topicC, and check to see it's the one sent to topicB
        val = recvC.receiveEvent()
        self.assertIsNotNone(val)
        ps = val.getPropertySet()
        name = ps.get("TOPIC")
        self.assertEqual(name, topicB)

        # receiving no more messages on topicC
        val = recvC.receiveEvent(1)
        self.assertIsNone(val)
    

def suite():
    """Returns a suite containing all the tests cases in this module."""
    tests.init()
    suites = []
    suites += unittest.makeSuite(CombinedReceiveEventTestCase)
    suites += unittest.makeSuite(tests.MemoryTestCase)
    return unittest.TestSuite(suites)

def run(shouldExit=False):
    """Run the tests."""
    tests.run(suite(), shouldExit)

if __name__ == "__main__":
    run(True)
