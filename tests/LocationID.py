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
import lsst.pex.logging as logging
import lsst.pex.policy as policy
from lsst.daf.base import PropertySet

class LocationIDTestCase(unittest.TestCase):
    # return the ip address in an integer
    def getHostAddr(self):
        
        hostname = socket.gethostname()
        (name, aliaslist, ipaddrlist) = socket.gethostbyname_ex(hostname)
        exp = 3
        addr = 0
        for byte in ipaddrlist[0].split('.'):
            addr = addr + (int(byte) * (256 ** exp))
            exp = exp -1
        return addr
        
    def testLocationID(self):
        eventSystem = events.EventSystem().getDefaultEventSystem()

        locationID = events.LocationID()

        localId = locationID.getLocalID()
        self.assertEqual(localId, 0)

        hostname = locationID.getHostName()
        self.assertEqual(hostname, socket.gethostname())
        processId = locationID.getProcessID()
        self.assertEqual(processId, os.getpid())

        locationID2 = events.LocationID()

        localId = locationID2.getLocalID()
        self.assertEqual(localId, 1)

        hostname = locationID.getHostName()
        self.assertEqual(hostname, socket.gethostname())
        processId = locationID2.getProcessID()
        self.assertEqual(processId, os.getpid())

        hostname = locationID2.getHostName()

        root = PropertySet()
        root.set("myname","myname")
        status = "my special status"
        root.set(events.Event.STATUS, status)

        statusEvent = events.StatusEvent("my runid", locationID2, root)

        topic = "mytopic_%s_%d" % (platform.node(), os.getpid())
        transmitter = events.EventTransmitter("lsst8.ncsa.illinois.edu", topic)
        sel = "%s = '%s'" % (events.StatusEvent.ORIG_HOSTNAME, hostname)
        #sel = "RUNID = '%s'" % "my runid"
        #sel = "PROCESSID = %d" % processId
        print topic
        print sel
        receiver = events.EventReceiver("lsst8.ncsa.illinois.edu", topic, sel)

        transmitter.publishEvent(statusEvent)
        returnedEvent = receiver.receiveEvent()

        print "done"

if __name__ == "__main__":
    unittest.main()
