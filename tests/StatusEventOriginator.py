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
from lsst.daf.base import PropertySet

#
# Send an event
#
class StatusEventOriginatorTestCase(unittest.TestCase):
    def sendEvent(self, brokerName, topic):
        trans = events.EventTransmitter(brokerName, topic)
        
        root = PropertySet()
        root.set("TOPIC",topic)
        root.set("myname","myname")
        root.set("STATUS", "my special status")
        root.set("RUNID","srptestrun")
        root.set("bazinga", "sheldon")
        
        originatorId = events.LocationID()
        event = events.StatusEvent("srptestrun", originatorId, root)
    
        # ok...now publish it
        trans.publishEvent(event)

    def testStatusEventOriginator(self):
        broker = "lsst8.ncsa.illinois.edu"
    
        host = platform.node()
        pid = os.getpid()
        topic = "test_events_12_%s_%d" % (host, pid)
    
        recv = events.EventReceiver(broker, topic)
    
        #
        # send a test event, and wait to receive it
        #
        self.sendEvent(broker, topic)
    
        val = recv.receiveEvent()
        assert val != None
        ps = val.getCustomPropertySet()
        ps = val.getPropertySet()
    
        eventSystem = events.EventSystem.getDefaultEventSystem()
        statusevent = eventSystem.castToStatusEvent(val)
   
        originatorID = statusevent.getOriginator()
        self.assertEqual(originatorID.getLocalID(), 0)
        self.assertEqual(originatorID.getProcessID(), os.getpid())
        self.assertEqual(platform.node(), originatorID.getHostName())

if __name__ == "__main__":
    unittest.main()
