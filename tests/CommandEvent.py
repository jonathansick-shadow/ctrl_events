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

import unittest

import os
import platform
import lsst.ctrl.events as events
import lsst.pex.policy as policy
from lsst.daf.base import PropertySet

#
# Command Event test
#
class CommandTestCase(unittest.TestCase):

    def testCommandEvent(self):
        broker = "lsst8.ncsa.illinois.edu"
        topic = "test_events_command_%s_%d" % (platform.node(), os.getpid())
    
        receiver = events.EventReceiver(broker, topic)
    
        trans = events.EventTransmitter(broker, topic)
    
        originatorId = events.LocationID()
        root = PropertySet()
        root.set("TOPIC",topic)
        root.set("myname","myname")
        root.set("STATUS", "my special status")
        event = events.StatusEvent("srptestrun", originatorId, root)
    
        statusOriginatorId = event.getOriginator()
        destinationID = events.LocationID(statusOriginatorId)
    
        commandOriginatorId = events.LocationID()
        root2 = PropertySet()
        root2.set("TOPIC",topic)
        root2.set("myname","myname2")
        root2.set("STATUS", "my special status2")
        event2 = events.CommandEvent("srptestrun", commandOriginatorId, destinationID, root2)
    
        trans.publishEvent(event2)
    
        val = receiver.receiveEvent()

        # be sure we received an event
        self.assertNotEqual(val, None)

        # get only the filterable properties and make sure we only get the names we expect
        fnames = val.getFilterablePropertyNames()
        self.assertTrue(len(fnames), 12)

        names = ['DEST_HOSTNAME', 'DEST_LOCALID', 'DEST_PROCESSID', 'EVENTTIME', 'ORIG_HOSTNAME', 'ORIG_LOCALID', 'ORIG_PROCESSID', 'PUBTIME', 'RUNID', 'STATUS', 'TOPIC', 'TYPE']
        for x in names:
            self.assertTrue(x in fnames)

        # get the whole PropertySet and make sure we only get the names we expect
        names.append('myname')
        ps = val.getPropertySet()
        self.assertEqual(ps.nameCount(), 13)
        for x in names:
            self.assertTrue(ps.exists(x))
    
        eventsystem = events.EventSystem.getDefaultEventSystem()
        commandEvent = eventsystem.castToCommandEvent(val)
        
        orig = commandEvent.getOriginator()
        self.assertEqual(orig.getLocalID(), 1)
        self.assertEqual(orig.getProcessID(), os.getpid())
        self.assertEqual(orig.getHostName(), platform.node())
        
        dest = commandEvent.getDestination()

        self.assertEqual(dest.getLocalID(), 0)
        self.assertEqual(dest.getProcessID(), os.getpid())
        self.assertEqual(dest.getHostName(), platform.node())

    def testFilterableCommandEvent(self):
        broker = "lsst8.ncsa.illinois.edu"
        topic = "test_events_command_%s_%d" % (platform.node(), os.getpid())
    
        receiver = events.EventReceiver(broker, topic)
    
        trans = events.EventTransmitter(broker, topic)
    
        originatorId = events.LocationID()
        root = PropertySet()
        root.set("TOPIC",topic)
        root.set("myname","myname")
        root.set("STATUS", "my special status")
        event = events.StatusEvent("srptestrun", originatorId, root)
    
        statusOriginatorId = event.getOriginator()
        destinationID = events.LocationID(statusOriginatorId)
    
        commandOriginatorId = events.LocationID()
        root2 = PropertySet()
        root2.set("TOPIC",topic)
        root2.set("myname","myname2")
        root2.set("STATUS", "my special status2")

        filterable = PropertySet()
        filterable.set("FOO", 12.3)
        filterable.set("BAR", .45)

        event2 = events.CommandEvent("srptestrun", commandOriginatorId, destinationID, root2, filterable)
    
        trans.publishEvent(event2)
    
        val = receiver.receiveEvent()

        # be sure we received an event
        self.assertNotEqual(val, None)

        # get only the filterable properties and make sure we only get the names we expect
        fnames = val.getFilterablePropertyNames()
        self.assertTrue(len(fnames), 14)

        names = ['DEST_HOSTNAME', 'DEST_LOCALID', 'DEST_PROCESSID', 'EVENTTIME', 'ORIG_HOSTNAME', 'ORIG_LOCALID', 'ORIG_PROCESSID', 'PUBTIME', 'RUNID', 'STATUS', 'TOPIC', 'TYPE', 'FOO', 'BAR']
        for x in names:
            self.assertTrue(x in fnames)

        # get the whole PropertySet and make sure we only get the names we expect
        names.append('myname')
        ps = val.getPropertySet()
        self.assertEqual(ps.nameCount(), 15)
        for x in names:
            self.assertTrue(ps.exists(x))
    
        eventsystem = events.EventSystem.getDefaultEventSystem()
        commandEvent = eventsystem.castToCommandEvent(val)
        
        orig = commandEvent.getOriginator()
        self.assertEqual(orig.getLocalID(), 3)
        self.assertEqual(orig.getProcessID(), os.getpid())
        self.assertEqual(orig.getHostName(), platform.node())
        
        dest = commandEvent.getDestination()

        self.assertEqual(dest.getLocalID(), 2)
        self.assertEqual(dest.getProcessID(), os.getpid())
        self.assertEqual(dest.getHostName(), platform.node())



if __name__ == "__main__":
    unittest.main()

