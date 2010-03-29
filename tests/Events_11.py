#!/usr/bin/env python

import time
import threading
import lsst.ctrl.events as events
from lsst.daf.base import PropertySet

#
# Send an event
#
def sendEvent(hostName, topic):
    trans = events.EventTransmitter(hostName, topic)
    
    root = PropertySet()
    root.set("TOPIC",topic)
    root.set("myname","myname")
    root.set("STATUS", "my special status")
    
    event = events.StatusEvent("srptestrun1", root)

    # ok...now publish it
    trans.publishEvent(event)

    event = events.StatusEvent("srptestrun2", root)

    # ok...now publish it
    trans.publishEvent(event)

if __name__ == "__main__":
    host = "fester.ncsa.uiuc.edu"
    topicA = "PIPELINE.A"
    topicB = "PIPELINE.B"
    topicC = "PIPELINE.*"

    yC = events.EventReceiver(host, topicC, "RUNID = 'srptestrun2'")

    #
    # send a test event, and wait to receive it
    #
    sendEvent(host, topicA)

    # we'll get the second event, not the first
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
