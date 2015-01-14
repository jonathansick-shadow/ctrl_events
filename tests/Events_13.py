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
# Send an event
#
class T451TestCase(unittest.TestCase):

    def sendEvent(self, brokerName, topic):
        trans = events.EventTransmitter(brokerName, topic)
    
        originatorId = events.LocationID()
    
        root = PropertySet()
        root.set("TOPIC",topic)
        root.set("myname","myname")
        root.set("STATUS", "my special status")
        
        event = events.StatusEvent("srptestrun", originatorId, root)
    
        print "creating STATUS EVENT:"
        self.printEvent(event)
    
        statusOriginatorId = event.getOriginator()
    
        destinationID = events.LocationID(statusOriginatorId)
    
        commandOriginatorId = events.LocationID()
    
        root2 = PropertySet()
        root2.set("TOPIC",topic)
        root2.set("myname","myname2")
        root2.set("STATUS", "my special status2")
        event = events.CommandEvent("srptestrun", commandOriginatorId, destinationID, root2)
    
        print "command event:"
        self.printEvent(event)
    
        trans.publishEvent(event)
    
    def printEvent(self, val):
        print "custom property names"
        print val.getCustomPropertyNames()
        print "Custom PropertySet"
        ps = val.getCustomPropertySet()
        print ps.toString()
        print
        print "filterable property names"
        print val.getFilterablePropertyNames()
    
        print "PropertySet"
        ps = val.getPropertySet()
        print ps.toString()
    
    def test451(self):
        broker = "lsst8.ncsa.illinois.edu"
        topicA = "test_events_13_%s_%d" % (platform.node(), os.getpid())
    
        receiver = events.EventReceiver(broker, topicA)
    
        #
        # send a test event, and wait to receive it
        #
        self.sendEvent(broker, topicA)
    
        val = receiver.receiveEvent()
        self.assertNotEqual(val, None)
        ps = val.getPropertySet()
        print "received ps:"
        print ps.toString()
    
        eventsystem = events.EventSystem.getDefaultEventSystem()
        commandEvent = eventsystem.castToCommandEvent(val)
        
        orig = commandEvent.getOriginator()
        
        print "orig: localID = ", orig.getLocalID()
        print "orig: processID = ", orig.getProcessID()
        print "orig: hostname = ", orig.getHostName()
    
        dest = commandEvent.getDestination()
    
        print "dest: localId = ", dest.getLocalID()
        print "dest: processId = ", dest.getProcessID()
        print "dest: hostname = ", dest.getHostName()

if __name__ == "__main__":
    unittest.main()

