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

class CommandTestCase(unittest.TestCase):
    """test CommandEvent"""

    def assertValid(self, val, names, origNum, destNum):
        """do a validity check on the values we received."""
        # get only the filterable properties and make sure we only get the names we expect
        fnames = val.getFilterablePropertyNames()
        self.assertTrue(len(fnames), len(names))

        for x in names:
            self.assertTrue(x in fnames)

        # get the whole PropertySet and make sure we only get the names we expect

        props = list(names)
        props.append('myname')
        ps = val.getPropertySet()
        self.assertEqual(ps.nameCount(), len(props))
        for x in names:
            self.assertTrue(ps.exists(x))
    
        eventsystem = events.EventSystem.getDefaultEventSystem()
        commandEvent = eventsystem.castToCommandEvent(val)
        
        # check originator values
        orig = commandEvent.getOriginator()
        self.assertEqual(orig.getLocalID(), origNum)
        self.assertEqual(orig.getProcessID(), os.getpid())
        self.assertEqual(orig.getHostName(), platform.node())
        
        dest = commandEvent.getDestination()

        # check destination values
        self.assertEqual(dest.getLocalID(), destNum)
        self.assertEqual(dest.getProcessID(), os.getpid())
        self.assertEqual(dest.getHostName(), platform.node())

    # test command event and command event with additional filterable properties.
    # this is run as one test, rather than two because of how LocationIds are
    # generated and the checks for the local values are order dependent
    @unittest.skipUnless(TestEnvironment().validTestDomain(), "not within valid domain")
    def testCommandEvent(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()
        thisHost = platform.node()

        topic = "test_events_command_%s_%d" % (thisHost, os.getpid())
    
        # send a command event
        receiver = events.EventReceiver(broker, topic)
        trans = events.EventTransmitter(broker, topic)
    
        originatorId = events.LocationId()
        root = PropertySet()
        root.set("TOPIC",topic)
        root.set("myname","myname")
        root.set("STATUS", "my special status")
        event = events.StatusEvent("srptestrun", originatorId, root)
    
        statusOriginatorId = event.getOriginator()
        destinationID = events.LocationId(statusOriginatorId)
    
        commandOriginatorID = events.LocationId()
        root2 = PropertySet()
        root2.set("TOPIC",topic)
        root2.set("myname","myname2")
        root2.set("STATUS", "my special status2")
        event2 = events.CommandEvent("srptestrun", commandOriginatorID, destinationID, root2)
    
        trans.publishEvent(event2)
        val = receiver.receiveEvent()

        # be sure we received an event
        self.assertNotEqual(val, None)
        # these are the filterable names we expect to see
        names = ['DEST_HOSTNAME', 'DEST_LOCALID', 'DEST_PROCESSID', 'EVENTTIME', 'ORIG_HOSTNAME', 
                    'ORIG_LOCALID', 'ORIG_PROCESSID', 'PUBTIME', 'RUNID', 'STATUS', 'TOPIC', 'TYPE']
        print dir(val)
        self.assertValid(val, names, commandOriginatorID.getLocalID(), destinationID.getLocalID())


        # send a command event with additional filterable properties
        receiver = events.EventReceiver(broker, topic)
    
        trans = events.EventTransmitter(broker, topic)
    
        originatorId = events.LocationId()
        event = events.StatusEvent("srptestrun", originatorId, root)
    
        statusOriginatorId = event.getOriginator()
        destinationID = events.LocationId(statusOriginatorId)
    
        commandOriginatorID = events.LocationId()

        filterable = PropertySet()
        filterable.set("FOO", 12.3)
        filterable.set("BAR", .45)

        event2 = events.CommandEvent("srptestrun", commandOriginatorID, destinationID, root2, filterable)
    
        trans.publishEvent(event2)
    
        val = receiver.receiveEvent()

        # be sure we received an event
        self.assertNotEqual(val, None)

        # these are the filterable names we expect to see
        names = ['DEST_HOSTNAME', 'DEST_LOCALID', 'DEST_PROCESSID', 'EVENTTIME', 'ORIG_HOSTNAME', 
                'ORIG_LOCALID', 'ORIG_PROCESSID', 'PUBTIME', 'RUNID', 'STATUS', 'TOPIC', 'TYPE', 'FOO', 'BAR']
        self.assertValid(val, names, commandOriginatorID.getLocalID(), destinationID.getLocalID())

def suite():
    """Returns a suite containing all the tests cases in this module."""
    tests.init()
    suites = []
    suites += unittest.makeSuite(CommandTestCase)
    suites += unittest.makeSuite(tests.MemoryTestCase)
    return unittest.TestSuite(suites)

def run(shouldExit=False):
    """Run the tests."""
    tests.run(suite(), shouldExit)

if __name__ == "__main__":
    run(True)
