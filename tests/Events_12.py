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


#!/usr/bin/env python

import os
import platform
import lsst.ctrl.events as events
from lsst.daf.base import PropertySet

#
# Send an event
#
def sendEvent(brokerName, topic):
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

if __name__ == "__main__":
    broker = "lsst8.ncsa.illinois.edu"

    host = platform.node()
    pid = os.getpid()
    topicA = "test_events_12_%s_%d" % (host, pid)

    yC = events.EventReceiver(broker, topicA)

    #
    # send a test event, and wait to receive it
    #
    sendEvent(broker, topicA)

    val = yC.receiveEvent()
    assert val != None
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

    eventSystem = events.EventSystem().getDefaultEventSystem()
    statusevent = eventSystem.castToStatusEvent(val)
    print "OriginatorId"
    originatorID = statusevent.getOriginatorId()
    print "localID", originatorID.getLocalID()
    print "processID", originatorID.getProcessID()
    print "IPAddress", originatorID.getIPAddress()
