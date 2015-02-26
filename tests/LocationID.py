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

import os, socket, struct, platform
import lsst.ctrl.events as events
import lsst.daf.base as base
import lsst.pex.exceptions
import lsst.pex.policy as policy
from lsst.daf.base import PropertySet

class LocationIDTestCase(unittest.TestCase):
        
    def testLocationID(self):
        eventSystem = events.EventSystem.getDefaultEventSystem()

        locationID = events.LocationID()

        # check to see if localID is 0
        localId = locationID.getLocalID()
        self.assertEqual(localId, 0)

        # check host name
        hostname = locationID.getHostName()
        self.assertEqual(hostname, socket.gethostname())

        # check process id
        processId = locationID.getProcessID()
        self.assertEqual(processId, os.getpid())

        locationID2 = events.LocationID()

        # check to see if localID is 1
        localId = locationID2.getLocalID()
        self.assertEqual(localId, 1)

        self.assertEqual(locationID.getLocalID(), 0)

        # check host name
        hostname = locationID.getHostName()
        self.assertEqual(hostname, socket.gethostname())

        # check process id
        processId = locationID2.getProcessID()
        self.assertEqual(processId, os.getpid())


        root = PropertySet()
        root.set("myname","myname")
        status = "my special status"
        root.set(events.Event.STATUS, status)

        statusEvent = events.StatusEvent("my runid", locationID, root)
        statusEvent2 = events.StatusEvent("my runid", locationID2, root)

        topic = "mytopic_%s_%d" % (platform.node(), os.getpid())
        transmitter = events.EventTransmitter("lsst8.ncsa.illinois.edu", topic)

        hostname = locationID2.getHostName()

        # create a receiver selector with the hostname, process id and local id
        sel = "%s = '%s' and %s = %d and %s = %d" % (events.StatusEvent.ORIG_HOSTNAME, hostname, events.StatusEvent.ORIG_PROCESSID, os.getpid(), events.StatusEvent.ORIG_LOCALID, 1)

        receiver = events.EventReceiver("lsst8.ncsa.illinois.edu", topic, sel)

        # transmit the events
        transmitter.publishEvent(statusEvent)
        transmitter.publishEvent(statusEvent2)

        # should receive event with with the process id
        returnedEvent = receiver.receiveEvent()
        ps = returnedEvent.getPropertySet()
        self.assertEqual(ps.get('ORIG_HOSTNAME'), hostname)
        self.assertEqual(ps.get('ORIG_PROCESSID'), os.getpid())
        self.assertEqual(ps.get('ORIG_LOCALID'), 1)

        # should NOT receive another event, because it was filtered out by
        # the broker
        returnedEvent2 = receiver.receiveEvent(1)
        self.assertEqual(returnedEvent2, None)
        

        print "done"

if __name__ == "__main__":
    unittest.main()
