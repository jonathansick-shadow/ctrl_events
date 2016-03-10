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
import lsst.ctrl.events as events
import lsst.daf.base as base
import lsst.utils.tests as tests
from testEnvironment import TestEnvironment

class EventQueuesTestCase(unittest.TestCase):
    """Test the EventTransmitter using queues"""

    def sendEvent(self, broker, topicName, port, value):
        root = base.PropertySet()
        root.addInt("FOO", value)
        trans = events.EventEnqueuer(broker, topicName, port)
        event = events.Event("myrunid", root)
        trans.publishEvent(event)

    @unittest.skipUnless(TestEnvironment().validTestDomain(), "not within valid domain")
    def testEventReceiverCreatedFirst(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()
        port = testEnv.getPort()
        thisHost = platform.node()

        topic = "test_events_queue1_%s_%d" % (thisHost, os.getpid())
        recv = events.EventDequeuer(broker, topic, port);

        # Integer tests

        #
        # send two test events
        #
        self.sendEvent(broker, topic, port, 300)
        self.sendEvent(broker, topic, port, 200)

        val = recv.receiveEvent()
        self.assertIsNotNone(val)

        # check to see we got 300 back
        ps = val.getPropertySet()
        self.assertEqual(ps.get("FOO"), 300)

        val = recv.receiveEvent()
        self.assertIsNotNone(val)

        # check to see we got 200 back
        ps = val.getPropertySet()
        self.assertEqual(ps.get("FOO"), 200)

        # check to see no other events are waiting
        val = recv.receiveEvent(1)
        self.assertIsNone(val)

    @unittest.skipUnless(TestEnvironment().validTestDomain(), "not within valid domain")
    def testEventTransmitterCreatedFirst(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()
        port = testEnv.getPort()
        thisHost = platform.node()

        topic = "test_events_queue2_%s_%d" % (thisHost, os.getpid())

        # Integer tests

        #
        # send two test events
        #
        self.sendEvent(broker, topic, port, 300)
        self.sendEvent(broker, topic, port, 200)

        recv = events.EventDequeuer(broker, topic, port);

        val = recv.receiveEvent()
        self.assertIsNotNone(val)

        # check to see we got 300 back
        ps = val.getPropertySet()
        self.assertEqual(ps.get("FOO"), 300)

        val = recv.receiveEvent()
        self.assertIsNotNone(val)

        # check to see we got 200 back
        ps = val.getPropertySet()
        self.assertEqual(ps.get("FOO"), 200)

        # check to see no other events are waiting
        val = recv.receiveEvent(1)
        self.assertIsNone(val)

def suite():
    """Returns a suite containing all the tests cases in this module."""
    tests.init()
    suites = []
    suites += unittest.makeSuite(EventQueuesTestCase)
    suites += unittest.makeSuite(tests.MemoryTestCase)
    return unittest.TestSuite(suites)

def run(shouldExit=False):
    """Run the tests."""
    tests.run(suite(), shouldExit)

if __name__ == "__main__":
    run(True)
