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

class EventSelectorTestCase(unittest.TestCase):
    """Test receiving events using the selector mechanism"""

    def sendEvent(self, runid, brokerName, topic):
        trans = events.EventTransmitter(brokerName, topic)
        
        root = PropertySet()
        root.set("TOPIC",topic)
        root.set("myname","myname")
        root.set("STATUS", "my special status")
        
        locationID = events.LocationID()
    
        event = events.StatusEvent(runid, locationID, root)
        # ok...now publish it
        trans.publishEvent(event)
    
    @unittest.skipUnless(TestEnvironment().validTestDomain(), "not within valid domain")
    def testEventSelector(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()
        thisHost = platform.node()

        host_pid = "%s_%d" % (thisHost, os.getpid())
    
        topic = "test_events_11_%s" % host_pid
    
        runid = 'test_runid_11_%d' % os.getpid()

        rec = events.EventReceiver(broker, topic, "RUNID = '%s'" % runid)
    
        #
        # send a test event, and wait to receive it
        #
        self.sendEvent(runid, broker, topic)
    
        # we'll get the second event, not the first
        val = rec.receiveEvent()
        self.assertNotEqual(val, None)
        ps = val.getPropertySet()
        self.assertTrue(ps.exists('RUNID'))
        self.assertEqual(ps.get('RUNID'),runid)

        self.sendEvent("invalid", broker, topic)
        # shouldn't receive anything else
        val2 = rec.receiveEvent(1)
        self.assertEqual(val2, None)

def suite():
    """Returns a suite containing all the tests cases in this module."""
    tests.init()
    suites = []
    suites += unittest.makeSuite(EventSelectorTestCase)
    suites += unittest.makeSuite(tests.MemoryTestCase)
    return unittest.TestSuite(suites)

def run(shouldExit=False):
    """Run the tests."""
    tests.run(suite(), shouldExit)

if __name__ == "__main__":
    run(True)
