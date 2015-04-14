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
from lsst.daf.base import PropertySet
import lsst.utils.tests as tests
from testEnvironment import TestEnvironment

class StatusEventOriginatorTestCase(unittest.TestCase):
    """Test StatusEvent Originator"""

    def sendEvent(self, brokerName, topic):
        """Send an Event."""
        trans = events.EventTransmitter(brokerName, topic)
        
        root = PropertySet()
        root.set("TOPIC",topic)
        root.set("myname","myname")
        root.set("STATUS", "my special status")
        root.set("RUNID","srptestrun")
        root.set("bazinga", "sheldon")
        
        originatorId = events.LocationId()
        event = events.StatusEvent("srptestrun", originatorId, root)
    
        # ok...now publish it
        trans.publishEvent(event)

    @unittest.skipUnless(TestEnvironment().validTestDomain(), "not within valid domain")
    def testStatusEventOriginator(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()
        thisHost = platform.node()

        topic = "test_events_12_%s_%d" % (thisHost, os.getpid())
    
        recv = events.EventReceiver(broker, topic)
    
        #
        # send a test event, and wait to receive it
        #
        self.sendEvent(broker, topic)
    
        val = recv.receiveEvent()
        assert val != None
    
        eventSystem = events.EventSystem.getDefaultEventSystem()
        statusevent = eventSystem.castToStatusEvent(val)
   
        originatorID = statusevent.getOriginator()
        self.assertEqual(originatorID.getLocalID(), 0)
        self.assertEqual(originatorID.getProcessID(), os.getpid())
        self.assertEqual(platform.node(), originatorID.getHostName())

def suite():
    """Returns a suite containing all the tests cases in this module."""
    tests.init()
    suites = []
    suites += unittest.makeSuite(StatusEventOriginatorTestCase)
    suites += unittest.makeSuite(tests.MemoryTestCase)
    return unittest.TestSuite(suites)

def run(shouldExit=False):
    """Run the tests."""
    tests.run(suite(), shouldExit)

if __name__ == "__main__":
    run(True)
