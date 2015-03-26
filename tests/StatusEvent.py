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

class StatusEventTestCase(unittest.TestCase):
    """Test StatusEvent"""

    def init(self):
        self.broker = "lsst8.ncsa.illinois.edu"

    def createTopicName(self, template):
        host = platform.node()
        pid = os.getpid()

        host_pid = "%s_%d" % (host, pid)
        return template % host_pid

    def createReceiver(self, topic):
        broker = "lsst8.ncsa.illinois.edu"
        receiver = events.EventReceiver(broker, topic)
        return receiver

    def sendPlainStatusEvent(self, topic, runID=None):
        trans = events.EventTransmitter(self.broker, topic)
        
        root = PropertySet()
        root.set("TOPIC",topic)
        root.set("myname","myname")
        root.set("STATUS", "my special status")
        root.set("logger.status", "my logger special status")
        root.set("logger.pid", "1")
        #root.set("logger.pid.foo", 1)
        #root.set("logger.pid.bar", 2)
        
        eventSystem = events.EventSystem.getDefaultEventSystem();
        locationID = eventSystem.createOriginatorId()
        if runID is None:
            event = events.StatusEvent(locationID, root)
        else:
            event = events.StatusEvent(runID, locationID, root)

        # ok...now publish it
        trans.publishEvent(event)

    def sendFilterableStatusEvent(self, topic, runID=None):
        trans = events.EventTransmitter(self.broker, topic)
        
        root = PropertySet()
        root.set("TOPIC",topic)
        root.set("myname","myname")
        root.set("STATUS", "my special status")

        filter = PropertySet()
        filter.set("FOO", "bar")
        filter.set("PLOUGH", 123)
        filter.set("PLOVER", 0.45)
        
        eventSystem = events.EventSystem.getDefaultEventSystem();
        locationID = eventSystem.createOriginatorId()
        if runID is None:
            event = events.StatusEvent(locationID, root, filter)
        else:
            event = events.StatusEvent(runID, locationID, root, filter)

        # ok...now publish it
        trans.publishEvent(event)

    def testPlainStatusEvent(self):
        self.init()
        topic = self.createTopicName("test_events_10_%s.A")
        receiver = self.createReceiver(topic)
        #
        # send a test event, and wait to receive it
        #
        self.sendPlainStatusEvent(topic)

        val = receiver.receiveEvent()

        self.assertNotEqual(val, None)
        values = ['EVENTTIME', 'ORIG_HOSTNAME', 'ORIG_LOCALID', 'ORIG_PROCESSID', 'PUBTIME', 'STATUS', 'TOPIC', 'TYPE']
        self.checkValidity(val, values, 2)

        self.assertNotEqual(val.getEventTime(), 0)
        self.assertNotEqual(val.getPubTime(), 0)
        self.assertGreater(val.getPubTime(), val.getEventTime())


    def testStatusEventWithRunID(self):
        self.init()
        topicA = self.createTopicName("test_events_10_%s.B")
        receiverA = self.createReceiver(topicA)

        #
        # send a test event, and wait to receive it
        #
        self.sendPlainStatusEvent(topicA, "test_runID_10")


        val = receiverA.receiveEvent()
        self.assertNotEqual(val, None)
        values = ['EVENTTIME', 'ORIG_HOSTNAME', 'ORIG_LOCALID', 'ORIG_PROCESSID', 'PUBTIME', 'STATUS', 'TOPIC', 'TYPE', 'RUNID']
        self.checkValidity(val, values, 2)

        self.assertNotEqual(val.getEventTime(), 0)
        self.assertNotEqual(val.getPubTime(), 0)
        self.assertGreater(val.getPubTime(), val.getEventTime())

    def testFilterableStatusEvent(self):
        self.init()
        topic = self.createTopicName("test_events_10_%s.C")

        receiver = self.createReceiver(topic)

        self.sendFilterableStatusEvent(topic)

        val = receiver.receiveEvent()

        self.assertNotEqual(val, None)

        # should only be ['EVENTTIME', 'FOO', 'ORIG_HOSTNAME', 'ORIG_LOCALID', 'ORIG_PROCESSID', 'PLOUGH', 'PLOVER', 'PUBTIME', 'STATUS', 'TOPIC', 'TYPE']
        # in some order
        values = ['EVENTTIME', 'FOO', 'ORIG_HOSTNAME', 'ORIG_LOCALID', 'ORIG_PROCESSID', 'PLOUGH', 'PLOVER', 'PUBTIME', 'STATUS', 'TOPIC', 'TYPE']
        self.checkValidity(val, values, 1)

    def testFilterableStatusEventWithRunID(self):
        self.init()
        topic = self.createTopicName("test_events_10_%s.D")

        receiver = self.createReceiver(topic)

        self.sendFilterableStatusEvent(topic, "test_runID_10")

        val = receiver.receiveEvent()

        # should receive an event
        self.assertNotEqual(val, None)
        values = ['EVENTTIME', 'FOO', 'ORIG_HOSTNAME', 'ORIG_LOCALID', 'ORIG_PROCESSID', 'PLOUGH', 'PLOVER', 'PUBTIME', 'RUNID', 'STATUS', 'TOPIC', 'TYPE']
        self.checkValidity(val, values, 1)


    def checkValidity(self, val, values, customCount):
        # get custom property names
        names = val.getCustomPropertyNames()

        # should only be one custom property name...
        self.assertEqual(len(names), customCount)

        # ...and that name should be "myname"
        self.assertEqual(names[0], "myname")


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
        print "Entire property set"
        print ps.toString()

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
