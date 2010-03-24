#!/usr/bin/env python

import threading
import lsst.ctrl.events as events
from lsst.daf.base import PropertySet

#
# Send an event
#
def sendEvent(hostName, topic):
    trans = events.EventTransmitter(hostName, topic)
    
    root = PropertySet()
    root.set("topic",topic)
    root.set("myname",topic)
    
    trans.publish(root)

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

    val = yC.receive()
    assert val != None
    print val.toString()

    val = yC.receive()
    assert val != None
    print val.toString()
