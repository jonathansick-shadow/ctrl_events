#!/usr/bin/env python

# 
# LSST Data Management System
# Copyright 2008, 2009, 2010 LSST Corporation.
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
# see <http://www.lsstcorp.org/LegalNotices/>.
#

import os
import platform
import lsst.ctrl.events as events
import lsst.pex.policy as policy
from lsst.daf.base import PropertySet

#
# Send an event
#
def sendEvent(brokerName, topic):
    trans = events.EventTransmitter(brokerName, topic)

    originatorId = events.OriginatorID()

    root = PropertySet()
    root.set("TOPIC",topic)
    root.set("myname","myname")
    root.set("STATUS", "my special status")
    
    event = events.StatusEvent("srptestrun", originatorId, root)

    print "creating STATUS EVENT:"
    printEvent(event)

    statusOriginatorId = event.getOriginatorId()

    destinationID = events.DestinationID(statusOriginatorId)

    commandOriginatorId = events.OriginatorID()

    root2 = PropertySet()
    root2.set("TOPIC",topic)
    root2.set("myname","myname2")
    root2.set("STATUS", "my special status2")
    event = events.CommandEvent("srptestrun", commandOriginatorId, destinationID, root2)

    print "command event:"
    printEvent(event)

    trans.publishEvent(event)

def printEvent(val):
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

if __name__ == "__main__":
    broker = "lsst8.ncsa.illinois.edu"
    topicA = "test_events_13_%s_%d" % (platform.node(), os.getpid())

    receiver = events.EventReceiver(broker, topicA)

    #
    # send a test event, and wait to receive it
    #
    sendEvent(broker, topicA)

    val = receiver.receiveEvent()
    assert val != None
    ps = val.getPropertySet()
    print "received ps:"
    print ps.toString()

    eventsystem = events.EventSystem.getDefaultEventSystem()
    commandEvent = eventsystem.castToCommandEvent(val)
    
    orig = commandEvent.getOriginatorId()
    
    print "orig_localId = ", orig.getLocalID()
    print "orig_processId = ", orig.getProcessID()
    print "orig_IPId = ", orig.getIPAddress()

    dest = commandEvent.getDestinationId()

    print "dest_localId = ", dest.getLocalID()
    print "dest_processId = ", dest.getProcessID()
    print "dest_IPId = ", dest.getIPAddress()
