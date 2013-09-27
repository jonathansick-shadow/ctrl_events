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


import lsst.ctrl.events as events
from lsst.daf.base import PropertySet
import lsst.pex.exceptions
import lsst.pex.logging as logging
import lsst.pex.policy as policy


if __name__ == "__main__":
    
    host = "lsst8.ncsa.illinois.edu"
    eventSystem = events.EventSystem().getDefaultEventSystem()

    # need to add a test where a transmitter is created when
    # useLocalSockets is true, but currently you can't do that
    # because adding booleans to Policy doesn't work (Trac #258)

    topic = "EventSystem_8_test"

    
    eventSystem.createReceiver(host,topic)
   
    eventSystem.createTransmitter(host, topic)

    root = PropertySet()
    root.set("test", 12)
    root.set("STATUS", "important stuff")
    event = events.Event("eventsystem_8", root)


    eventSystem.publishEvent(topic, event)

    receivedEvent = eventSystem.receiveEvent(topic)

    assert receivedEvent != None
    print "custom property names"
    print receivedEvent.getCustomPropertyNames()
    print "Custom PropertySet"
    ps = receivedEvent.getCustomPropertySet()
    print ps.toString()
    print
    print "filterable property names"
    print receivedEvent.getFilterablePropertyNames()

    print "PropertySet"
    ps = receivedEvent.getPropertySet()
    print ps.toString()



