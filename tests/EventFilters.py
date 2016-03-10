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

class EventFiltersTestCase(unittest.TestCase):
    """A test case user specified filterable events"""

    def testEventFilters(self):
        runid = "testrunid"
        status = "my special status"
        root = PropertySet()
        MYNAME = "myname"
        root.set(MYNAME, MYNAME)
        root.set(events.Event.STATUS, status)

        filterable = PropertySet()
        filterable.set("FOO", "bar")
        filterable.set("XYZZY", 123)
        
        event = events.Event(runid, root, filterable)
    
        # return the property set
        props = event.getPropertySet()
        self.assertGreater(props.get(events.Event.EVENTTIME), 0)
        self.assertEqual(props.get(events.Event.PUBTIME), 0)
        self.assertEqual(props.get(events.Event.RUNID), runid)
        self.assertEqual(props.get(events.Event.STATUS), status)
        self.assertEqual(props.get(events.Event.TOPIC), events.Event.UNINITIALIZED)
        self.assertEqual(props.get(events.Event.TYPE), events.EventTypes.EVENT)
        self.assertEqual(props.get(MYNAME), MYNAME)
    
    
        filterableNames = event.getFilterablePropertyNames()
        
        filterableNames.remove(events.Event.EVENTTIME)
        filterableNames.remove(events.Event.PUBTIME)
        filterableNames.remove(events.Event.RUNID)
        filterableNames.remove(events.Event.STATUS)
        filterableNames.remove(events.Event.TOPIC)
        filterableNames.remove(events.Event.TYPE)
        # also get the two from "filterable"
        filterableNames.remove("FOO")
        filterableNames.remove("XYZZY")
        self.assertEqual(len(filterableNames), 0)
    
        customNames = event.getCustomPropertyNames()
    
        self.assertEqual(len(customNames), 1)
        self.assertEqual(customNames[0], MYNAME)
    
        eventTime = event.getEventTime()
        self.assertEqual(event.getPubTime(),0)
    
        self.assertEqual(event.getRunId(), runid)
        self.assertEqual(event.getType(), events.EventTypes.EVENT)
        self.assertEqual(event.getStatus(), status)
    
        # check to be sure we really update the time
        event.updateEventTime()
        self.assertLess(eventTime, event.getEventTime())
    
        # set the time to zero, and make sure we get back the same time.
        event.setEventTime(0)
        self.assertEqual(event.getEventTime(), 0)
    
        # reset the time to eventTime, and make sure we get back the same time.
        event.setEventTime(eventTime)
        self.assertEqual(event.getEventTime(), eventTime)


    @unittest.skipUnless(TestEnvironment().validTestDomain(), "not within valid domain")
    def testFilterableSendEvent(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()
        thisHost = platform.node()

        topic = "test_events_filters_%s_%d" % (thisHost, os.getpid())
    
        runId = "test_filters_runid"
        recv = events.EventReceiver(broker, topic)
    
        trans = events.EventTransmitter(broker, topic)
        
        root = PropertySet()
    
        DATE = "date"
        DATE_VAL = "2007-07-01T14:28:32.546012"
        root.set(DATE, DATE_VAL)
    
        BLANK = "blank"
        BLANK_VAL = ""
        root.set(BLANK, BLANK_VAL)
    
        PID = "pid"
        PID_VAL = os.getpid()
        root.setInt(PID, PID_VAL)
    
        HOST = "host"
        HOST_VAL = "lsstcorp.org"
        root.set(HOST, HOST_VAL)
    
        IP = "ip"
        IP_VAL = "1.2.3.4"
        root.set(IP, IP_VAL)
    
        EVNT = "evnt"
        EVNT_VAL = "test"
        root.set(EVNT, EVNT_VAL)
    
        MISC1 = "misc1"
        MISC1_VAL = "data 1"
        root.set(MISC1, MISC1_VAL)
    
        MISC2 = "misc2"
        MISC2_VAL = "data 2"
        root.set(MISC2, MISC2_VAL)
    
        MISC3 = "misc3"
        MISC3_VAL = ""
        root.set(MISC3, MISC3_VAL)
    
        DATA = "data"
        DATA_VAL = 3.14
        root.setDouble(DATA, DATA_VAL)

        filterable = PropertySet()
        filterable.set("FOO", "bar")
        filterable.set("XYZZY", 123)
        filterable.set("PLOUGH", 0.867)
        
        event = events.Event(runId, root, filterable)
        trans.publishEvent(event)
    
    
        val = recv.receiveEvent()
        self.assertIsNotNone(val)
    
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
        self.assertEqual(ps.get(events.Event.RUNID), runId)
        self.assertEqual(ps.get(events.Event.STATUS), "unknown")
        self.assertEqual(ps.get(events.Event.TOPIC), topic)
        self.assertEqual(ps.get(events.Event.TYPE), events.EventTypes.EVENT)
        self.assertEqual(ps.get("FOO"), "bar")
        self.assertEqual(ps.get("XYZZY"), 123)
        self.assertEqual(ps.get("PLOUGH"), 0.867)


        names = val.getFilterablePropertyNames()

        values = ['EVENTTIME', 'FOO', 'PLOUGH', 'PUBTIME', 'RUNID', 'STATUS', 'TOPIC', 'TYPE', 'XYZZY']

        for x in values:
            self.assertTrue(x in names)
        
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
    suites += unittest.makeSuite(EventFiltersTestCase)
    suites += unittest.makeSuite(tests.MemoryTestCase)
    return unittest.TestSuite(suites)

def run(shouldExit=False):
    """Run the tests."""
    tests.run(suite(), shouldExit)

if __name__ == "__main__":
    run(True)
