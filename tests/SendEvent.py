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


class SendEventTestCase(unittest.TestCase):
    """Test sending events"""

    @unittest.skipUnless(TestEnvironment().validTestDomain(), "not within valid domain")
    def testSendEvent(self):
        """Send an Event"""
        testEnv = TestEnvironment()
        self.thisHost = platform.node()
        self.broker = testEnv.getBroker()

        topic = "test_events_3_%s_%d" % (self.thisHost, os.getpid())

        runID = "test3_runid"
        recv = events.EventReceiver(self.broker, topic)

        trans = events.EventTransmitter(self.broker, topic)

        DATE = "date"
        DATE_VAL = "2007-07-01T14:28:32.546012"

        BLANK = "blank"
        BLANK_VAL = ""

        PID = "pid"
        PID_VAL = os.getpid()

        HOST = "host"
        HOST_VAL = "lsstcorp.org"

        IP = "ip"
        IP_VAL = "1.2.3.4"

        EVNT = "evnt"
        EVNT_VAL = "test"

        MISC1 = "misc1"
        MISC1_VAL = "data 1"

        MISC2 = "misc2"
        MISC2_VAL = "data 2"

        MISC3 = "misc3"
        MISC3_VAL = ""

        DATA = "data"
        DATA_VAL = 3.14

        root = PropertySet()
        root.set(DATE, DATE_VAL)
        root.set(BLANK, BLANK_VAL)
        root.setInt(PID, PID_VAL)
        root.set(HOST, HOST_VAL)
        root.set(IP, IP_VAL)
        root.set(EVNT, EVNT_VAL)
        root.set(MISC1, MISC1_VAL)
        root.set(MISC2, MISC2_VAL)
        root.set(MISC3, MISC3_VAL)
        root.setDouble(DATA, DATA_VAL)

        event = events.Event(runID, root)
        trans.publishEvent(event)

        val = recv.receiveEvent()
        self.assertIsNotNone(val)

        # check the validity of all sent values
        ps = val.getPropertySet()
        self.assertEqual(ps.get(DATE), DATE_VAL)
        self.assertEqual(ps.get(BLANK), BLANK_VAL)
        self.assertEqual(ps.get(PID), PID_VAL)
        self.assertEqual(ps.get(HOST), HOST_VAL)
        self.assertEqual(ps.get(IP), IP_VAL)
        self.assertEqual(ps.get(EVNT), EVNT_VAL)
        self.assertEqual(ps.get(MISC1), MISC1_VAL)
        self.assertEqual(ps.get(MISC2), MISC2_VAL)
        self.assertEqual(ps.get(MISC3), MISC3_VAL)
        self.assertEqual(ps.get(DATA), DATA_VAL)

        self.assertGreater(ps.get(events.Event.EVENTTIME), 0)
        self.assertGreater(ps.get(events.Event.PUBTIME), 0)
        self.assertEqual(ps.get(events.Event.RUNID), runID)
        self.assertEqual(ps.get(events.Event.STATUS), "unknown")
        self.assertEqual(ps.get(events.Event.TOPIC), topic)
        self.assertEqual(ps.get(events.Event.TYPE), events.EventTypes.EVENT)

        #
        # wait a short time to receive an event.  none was sent, so we should
        # time out and confirm that we didn't get anything
        #
        val = recv.receiveEvent(1000)
        self.assertIsNone(val)


def suite():
    """Returns a suite containing all the tests cases in this module."""
    tests.init()
    suites = []
    suites += unittest.makeSuite(SendEventTestCase)
    suites += unittest.makeSuite(tests.MemoryTestCase)
    return unittest.TestSuite(suites)


def run(shouldExit=False):
    """Run the tests."""
    tests.run(suite(), shouldExit)

if __name__ == "__main__":
    run(True)
