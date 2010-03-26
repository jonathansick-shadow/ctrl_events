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
    
    event = events.Event("srptestrun", root)

    # wait a short time so we can see the difference between the time 
    # the event is created and the time it is published
    time.sleep(2)

    # ok...now publish it
    trans.publishEvent(event)

if __name__ == "__main__":
    host = "fester.ncsa.uiuc.edu"
    topicA = "PIPELINE.A"
    topicB = "PIPELINE.B"
    topicC = "PIPELINE.*"

    yC = events.EventReceiver(host, topicC)

    #
    # send a test event, and wait to receive it
    #
    sendEvent(host, topicA)
    sendEvent(host, topicB)

    val = yC.receiveEvent()
    assert val != None
    print "eventTime = ",val.getEventTime()
    print "eventDate = ",val.getEventDate()
    print "pubTime = ",val.getPubTime()
    print "pubDate = ",val.getPubDate()

    val = yC.receiveEvent()
    assert val != None

