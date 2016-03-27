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


class StatusEventTestCase(unittest.TestCase):
    """Test StatusEvent"""

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
        root.set(events.Event.TOPIC, topic)
        root.set("myname", "myname")
        root.set(events.Event.STATUS, "my special status")
        root.set("logger.status", "my logger special status")
        root.set("logger.pid", "1")

        eventSystem = events.EventSystem.getDefaultEventSystem()
        locationID = eventSystem.createOriginatorId()
        if runID is None:
            event = events.StatusEvent(locationID, root)
        else:
            event = events.StatusEvent(runID, locationID, root)

        # ok...now publish it
        trans.publishEvent(event)

    def sendFilterableStatusEvent(self, broker, topic, runID=None):
        trans = events.EventTransmitter(broker, topic)

        root = PropertySet()
        root.set(events.Event.TOPIC, topic)
        root.set("myname", "myname")
        root.set(events.Event.STATUS, "my special status")

        filter = PropertySet()
        filter.set("FOO", "bar")
        filter.set("PLOUGH", 123)
        filter.set("PLOVER", 0.45)

        eventSystem = events.EventSystem.getDefaultEventSystem()
        locationID = eventSystem.createOriginatorId()
        if runID is None:
            event = events.StatusEvent(locationID, root, filter)
        else:
            event = events.StatusEvent(runID, locationID, root, filter)

        # ok...now publish it
        trans.publishEvent(event)

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

        self.assertIsNotNone(val)
        values = [events.Event.EVENTTIME, events.StatusEvent.ORIG_HOSTNAME, events.StatusEvent.ORIG_LOCALID,
                  events.StatusEvent.ORIG_PROCESSID, events.Event.PUBTIME, events.Event.STATUS, events.Event.TOPIC, events.Event.TYPE]
        customValues = ['myname', 'logger']
        self.assertValid(val, values, customValues)

        self.assertNotEqual(val.getEventTime(), 0)
        self.assertNotEqual(val.getPubTime(), 0)
        self.assertGreater(val.getPubTime(), val.getEventTime())
        self.assertEqual(val.getType(), events.EventTypes.STATUS)

    @unittest.skipUnless(TestEnvironment().validTestDomain(), "not within valid domain")
    def testStatusEventWithRunID(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()

        topicA = self.createTopicName("test_events_10_%s.B")
        receiverA = self.createReceiver(broker, topicA)

        #
        # send a test event, and wait to receive it
        #
        self.sendPlainStatusEvent(broker, topicA, "test_runID_10")

        val = receiverA.receiveStatusEvent()
        self.assertIsNotNone(val)
        values = [events.Event.EVENTTIME, events.StatusEvent.ORIG_HOSTNAME, events.StatusEvent.ORIG_LOCALID, events.StatusEvent.ORIG_PROCESSID,
                  events.Event.PUBTIME, events.Event.STATUS, events.Event.TOPIC, events.Event.TYPE, events.Event.RUNID]
        customValues = ['myname', 'logger']
        self.assertValid(val, values, customValues)

        self.assertNotEqual(val.getEventTime(), 0)
        self.assertNotEqual(val.getPubTime(), 0)
        self.assertGreater(val.getPubTime(), val.getEventTime())

    @unittest.skipUnless(TestEnvironment().validTestDomain(), "not within valid domain")
    def testFilterableStatusEvent(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()

        topic = self.createTopicName("test_events_10_%s.C")

        receiver = self.createReceiver(broker, topic)

        self.sendFilterableStatusEvent(broker, topic)

        val = receiver.receiveEvent()

        self.assertIsNotNone(val)

        values = [events.Event.EVENTTIME, 'FOO', events.StatusEvent.ORIG_HOSTNAME, events.StatusEvent.ORIG_LOCALID,
                  events.StatusEvent.ORIG_PROCESSID, 'PLOUGH', 'PLOVER', events.Event.PUBTIME, events.Event.STATUS, events.Event.TOPIC, events.Event.TYPE]
        customValues = ['myname']
        self.assertValid(val, values, customValues)

    @unittest.skipUnless(TestEnvironment().validTestDomain(), "not within valid domain")
    def testFilterableStatusEventWithRunID(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()

        topic = self.createTopicName("test_events_10_%s.D")

        receiver = self.createReceiver(broker, topic)

        self.sendFilterableStatusEvent(broker, topic, "test_runID_10")

        val = receiver.receiveEvent()

        # should receive an event
        self.assertIsNotNone(val)
        values = [events.Event.EVENTTIME, 'FOO', events.StatusEvent.ORIG_HOSTNAME, events.StatusEvent.ORIG_LOCALID, events.StatusEvent.ORIG_PROCESSID,
                  'PLOUGH', 'PLOVER', events.Event.PUBTIME, events.Event.RUNID, events.Event.STATUS, events.Event.TOPIC, events.Event.TYPE]
        customValues = ['myname']
        self.assertValid(val, values, customValues)

    def assertValid(self, val, values, customValues):
        customCount = len(customValues)
        # get custom property names
        names = val.getCustomPropertyNames()

        # should only be the assumed custom property names...
        self.assertEqual(len(names), customCount)

        for i in range(len(customValues)):
            self.assertTrue(customValues[i] in names)

        # get custom property set
        ps = val.getCustomPropertySet()

        # should only be one custom property name...
        self.assertEqual(ps.nameCount(), customCount)

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

        for x in allValues:
            self.assertTrue(ps.exists(x))


def suite():
    """Returns a suite containing all the tests cases in this module."""
    tests.init()
    suites = []
    suites += unittest.makeSuite(StatusEventTestCase)
    suites += unittest.makeSuite(tests.MemoryTestCase)
    return unittest.TestSuite(suites)


def run(shouldExit=False):
    """Run the tests."""
    tests.run(suite(), shouldExit)

if __name__ == "__main__":
    run(True)
