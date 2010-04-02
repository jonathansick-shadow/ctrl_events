#!/usr/bin/env python

import lsst.ctrl.events as events
from lsst.daf.base import PropertySet
import lsst.pex.exceptions
import lsst.pex.logging as logging
import lsst.pex.policy as policy


if __name__ == "__main__":
    
    host = "lsst8.ncsa.uiuc.edu"
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



