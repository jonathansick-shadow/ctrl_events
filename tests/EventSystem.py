#!/usr/bin/env python

#
# LSST Data Management System
#
# Copyright 2008-2016  AURA/LSST.
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
import lsst.pex.exceptions as ex
import lsst.ctrl.events as events
from lsst.daf.base import PropertySet
import lsst.utils.tests as tests
from testEnvironment import TestEnvironment

class EventSystemTestCase(unittest.TestCase):
    """test EventSystem"""

    def getDataSet(self):
        data = [("This", 1), ("is", 2.3), ("a", "four"), ("test", (5, 6, 7, 8, 9))]
        return data

    def createDestination(self, prefix):
        return prefix + "_%s_%d" % (platform.node(), os.getpid())

    def createPropertySet(self):
        data = self.getDataSet()

        prop = PropertySet()
        for x in data:
            prop.set(x[0],x[1])

        return prop

    def verifyPropertySet(self, prop):
        data = self.getDataSet()
        for x in data:
            self.assertEqual(prop.get(x[0]), x[1])

    def performEventTest(self, destination):
        eventSystem = events.EventSystem.getDefaultEventSystem()

        prop = self.createPropertySet()

        # create and publish an event without a runid
        event = events.Event(prop)
        eventSystem.publishEvent(destination, event)

        # should receive the event
        receivedEvent = eventSystem.receiveEvent(destination)
        self.assertIsNotNone(receivedEvent)

        # should be able to get the PropertySet we sent
        recvProp = receivedEvent.getPropertySet()
        self.assertIsNotNone(recvProp)

        # verify PropertySet contents
        # shouldn't have a RUNID
        with self.assertRaises(ex.Exception):
            recvProp.get("RUNID")

        self.verifyPropertySet(recvProp)

    def performEventSelectorTest(self, runid, destination):
        eventSystem = events.EventSystem.getDefaultEventSystem()

        prop = self.createPropertySet()

        # create and publish an event without a runid
        event = events.Event(prop)
        eventSystem.publishEvent(destination, event)

        # attempt to receive the event, which shouldn't arrive, because the
        # destination we send this to has a RUNID selector associated with
        # it, which this event didn't have.
        receivedEvent = eventSystem.receiveEvent(destination, 20)
        self.assertIsNone(receivedEvent)

        # create and publish an event with a runid
        event = events.Event(runid, prop)
        eventSystem.publishEvent(destination, event)

        # attempt to receive the event, which SHOULD arrive, because we
        # specified the RUNID
        receivedEvent = eventSystem.receiveEvent(destination, 20)
        self.assertIsNotNone(receivedEvent)

        # should be able to get the PropertySet we sent
        recvProp = receivedEvent.getPropertySet()
        self.assertIsNotNone(recvProp)

        # verify the RUNID
        self.assertEqual(recvProp.get("RUNID"), runid)

        # verify the other PropertySet contents
        self.verifyPropertySet(recvProp)

    # test command event and command event with additional filterable properties.
    # this is run as one test, rather than two because of how LocationIds are
    # generated and the checks for the local values are order dependent
    @unittest.skipUnless(TestEnvironment().validTestDomain(), "not within valid domain")
    def testTopicDefault(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()

        eventSystem = events.EventSystem.getDefaultEventSystem()

        # test with default port
        topic1 = self.createDestination("test_topics1")
        eventSystem.createTransmitter(broker, topic1)
        eventSystem.createReceiver(broker, topic1)

        self.performEventTest(topic1)

    def testTopicSpecified(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()

        eventSystem = events.EventSystem.getDefaultEventSystem()

        # test with specified port
        topic2 = self.createDestination("test_topics2")

        eventSystem.createTransmitter(broker, topic2, testEnv.getPort())
        eventSystem.createReceiver(broker, topic2, testEnv.getPort())

        self.performEventTest(topic2)

    def testTopicSelector(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()

        eventSystem = events.EventSystem.getDefaultEventSystem()

        topic3 = self.createDestination("test_topics3")
        runid = "id007"

        eventSystem.createTransmitter(broker, topic3)
        eventSystem.createReceiver(broker, topic3, "RUNID = '%s'" % runid)
        self.performEventSelectorTest(runid, topic3)

    def testQueueOrder(self):
        # this tests the order in which the sender and receiver are created
        # and event is created and sent before the receiver exists, and the
        # receiver should still get the message

        testEnv = TestEnvironment()
        broker = testEnv.getBroker()

        eventSystem = events.EventSystem.getDefaultEventSystem()

        # test with default port
        queue = self.createDestination("test_queues4")
        eventSystem = events.EventSystem.getDefaultEventSystem()
        eventSystem.createEnqueuer(broker, queue)

        prop = self.createPropertySet()

        # create and publish an event without a runid, BEFORE
        # creating a receiver
        event = events.Event(prop)
        eventSystem.publishEvent(queue, event)

        eventSystem.createDequeuer(broker, queue)

        # should receive the event
        receivedEvent = eventSystem.receiveEvent(queue)
        self.assertIsNotNone(receivedEvent)

        # should be able to get the PropertySet we sent
        recvProp = receivedEvent.getPropertySet()
        self.assertIsNotNone(recvProp)

        # verify PropertySet contents
        self.verifyPropertySet(recvProp)

    def testQueueDefaultPort(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()
        thisHost = platform.node()
        eventSystem = events.EventSystem.getDefaultEventSystem()

        # test with default port
        queue1 = self.createDestination("test_queues1")
        eventSystem = events.EventSystem.getDefaultEventSystem()
        eventSystem.createEnqueuer(broker, queue1)
        eventSystem.createDequeuer(broker, queue1)

        self.performEventTest(queue1)

    def testQueueSpecifiedPort(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()
        eventSystem = events.EventSystem.getDefaultEventSystem()

        thisHost = platform.node()
        # test with specified port
        queue2 = self.createDestination("test_queues2")
        eventSystem.createEnqueuer(broker, queue2, testEnv.getPort())
        eventSystem.createDequeuer(broker, queue2, testEnv.getPort())

        self.performEventTest(queue2)

    def testQueueSelector(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()
        thisHost = platform.node()
        eventSystem = events.EventSystem.getDefaultEventSystem()

        queue3 = self.createDestination("test_queue3")
        runid = "id007"

        eventSystem.createEnqueuer(broker, queue3)
        eventSystem.createDequeuer(broker, queue3, "RUNID = '%s'" % runid)
        self.performEventSelectorTest(runid, queue3)


def suite():
    """Returns a suite containing all the tests cases in this module."""
    tests.init()
    suites = []
    suites += unittest.makeSuite(EventSystemTestCase)
    suites += unittest.makeSuite(tests.MemoryTestCase)
    return unittest.TestSuite(suites)

def run(shouldExit=False):
    """Run the tests."""
    tests.run(suite(), shouldExit)

if __name__ == "__main__":
    run(True)
