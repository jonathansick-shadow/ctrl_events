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
    root.set("RUNID","srptestrun")
    
    eventSystem = events.EventSystem.getDefaultEventSystem();
    originatorId = eventSystem.createOriginatorId()
    event = events.StatusEvent("srptestrun", originatorId, root)


    # ok...now publish it
    trans.publishEvent(event)

if __name__ == "__main__":
    host = "lsst8.ncsa.uiuc.edu"
    topicA = "PIPELINE.A"
    topicB = "PIPELINE.B"
    topicC = "PIPELINE.*"

    yC = events.EventReceiver(host, topicC)

    #
    # send a test event, and wait to receive it
    #
    sendEvent(host, topicA)

    # wait a short time so we can see the difference between the time 
    # the event is created and the time it is published
    time.sleep(1)

    sendEvent(host, topicB)

    val = yC.receiveEvent()
    assert val != None
    print "eventTime = ",val.getEventTime()
    print "eventDate = ",val.getEventDate()
    print "pubTime = ",val.getPubTime()
    print "pubDate = ",val.getPubDate()

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
