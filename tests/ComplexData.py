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
import lsst.pex.exceptions as ex
import lsst.utils.tests as tests
from testEnvironment import TestEnvironment

class ComplexDataTestCase(unittest.TestCase):
    """Test complex PropertySet sending"""

    def createTopicName(self, template):
        host = platform.node()
        pid = os.getpid()

        host_pid = "%s_%d" % (host, pid)
        return template % host_pid

    def createReceiver(self, broker, topic):
        receiver = events.EventReceiver(broker, topic)
        return receiver

    def sendPlainStatusEvent(self, broker, topic, runID=None):
        trans = events.EventTransmitter(broker, topic)
        
        root = PropertySet()
        root.set("TOPIC",topic)
        root.set("myname","myname")
        root.set("STATUS", "my special status")
        root.set("value", 12)
        root.set("logger.status", "my logger special status")
        root.set("logger.name", "myname")
        root.set("logger.pid.xyzzy", 1)
        root.set("logger.pid.plover", 3.14)
        root.set("logger.pid.plugh", "a hollow voice says")
        
        eventSystem = events.EventSystem.getDefaultEventSystem();
        locationID = eventSystem.createOriginatorId()
        if runID is None:
            event = events.StatusEvent(locationID, root)
        else:
            event = events.StatusEvent(runID, locationID, root)

        # ok...now publish it
        trans.publishEvent(event)

    def sendFilterableStatusEvent(self, broker, topic):
        trans = events.EventTransmitter(broker, topic)

        root = PropertySet()
        root.set("TOPIC",topic)
        root.set("myname","myname")
        root.set("STATUS", "my special status")

        filter = PropertySet()
        filter.set("FOO", "bar")
        filter.set("PLOUGH", 123)

        eventSystem = events.EventSystem.getDefaultEventSystem();
        locationID = eventSystem.createOriginatorId()
        event = events.StatusEvent(locationID, root, filter)

        # ok...now publish it
        trans.publishEvent(event)

    @unittest.skipUnless(TestEnvironment().validTestDomain(), "not within valid domain")
    def testFilterableStatusEvent(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()

        topic = self.createTopicName("test_events_10_%s.B")
        receiver = self.createReceiver(broker, topic)
        #
        # send a test event, and wait to receive it
        #
        self.sendFilterableStatusEvent(broker, topic)
        val = receiver.receiveEvent()

        self.assertNotEqual(val, None)

    @unittest.skipUnless(TestEnvironment().validTestDomain(), "not within valid domain")
    def testPlainStatusEvent(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()

        topic = self.createTopicName("test_events_10_%s.A")
        receiver = self.createReceiver(broker, topic)
        #
        # send a test event, and wait to receive it
        #
        self.sendPlainStatusEvent(broker, topic)

        val = receiver.receiveEvent()

        self.assertNotEqual(val, None)
        values = ['EVENTTIME', 'ORIG_HOSTNAME', 'ORIG_LOCALID', 'ORIG_PROCESSID', 'PUBTIME', 'STATUS', 'TOPIC', 'TYPE']
        customValues = ['logger', 'myname', 'value']
        self.checkValidity(val, values, customValues)

        self.assertNotEqual(val.getEventTime(), 0)
        self.assertNotEqual(val.getPubTime(), 0)
        self.assertGreater(val.getPubTime(), val.getEventTime())

    @unittest.skipUnless(TestEnvironment().validTestDomain(), "not within valid domain")
    def testIllegalFilterableStatusEvent(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()

        topic = self.createTopicName("test_events_10_%s.A")

        #
        # send a test event, and wait to receive it
        #
        trans = events.EventTransmitter(broker, topic)

        root = PropertySet()
        root.set("TOPIC",topic)
        root.set("myname","myname")
        root.set("STATUS", "my special status")

        filter = PropertySet()
        filter.set("FOO", "bar")
        filter.set("PLOUGH", 123)
        filter.set("PLOVER.FIRST", 1)
        filter.set("PLOVER.SECOND", 2)

        eventSystem = events.EventSystem.getDefaultEventSystem();
        locationID = eventSystem.createOriginatorId()
        event = events.StatusEvent(locationID, root, filter)

        self.assertRaises(ex.Exception, trans.publishEvent, event)

    def checkValidity(self, val, values, customValues):

        # get custom property names
        names = val.getCustomPropertyNames()

        # should only be two custom property names...
        self.assertEqual(len(names), len(customValues))

        for i in range(len(customValues)):
            self.assertTrue(customValues[i] in names)


        # get custom property set
        ps = val.getCustomPropertySet()

        self.assertEqual(ps.nameCount(), len(customValues))

        # ...and that should be "myname"
        self.assertTrue(ps.exists("myname"))


        # check filterable property names
        names = val.getFilterablePropertyNames()

        # should have len(values)
        self.assertEqual(len(names), len(values))

        for x in values:
            self.assertTrue(x in names)

        # get the entire property set
        ps = val.getPropertySet()

        allValues = list(values)
        allValues.append('myname')
        allValues.append('value')
        allValues.append('logger.status')

        self.assertEqual(ps.nameCount(), len(allValues))
        for x in allValues:
            self.assertTrue(ps.exists(x))

def suite():
    """Returns a suite containing all the tests cases in this module."""
    tests.init()
    suites = []
    suites += unittest.makeSuite(ComplexDataTestCase)
    suites += unittest.makeSuite(tests.MemoryTestCase)
    return unittest.TestSuite(suites)

def run(shouldExit=False):
    """Run the tests."""
    tests.run(suite(), shouldExit)

if __name__ == "__main__":
    run(True)
