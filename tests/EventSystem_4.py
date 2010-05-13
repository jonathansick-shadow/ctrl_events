#!/usr/bin/env python

import threading
import lsst.ctrl.events as events
import lsst.daf.base as base
import time


#
# Send an event
#
def sendEvent(topicName):
    
    root = base.PropertySet()
    root.set("DATE","2007-07-01T14:28:32.546012")
    root.setInt("PID",200)
    root.set("HOST","lsst8.ncsa.uiuc.edu")
    root.set("IP","141.142.220.44")
    root.set("EVNT","test")
    root.set("misc1","data 1")
    root.set("misc2","data 2")
    root.setFloat("float_value", 3.14)
    
    eventSystem = events.EventSystem.getDefaultEventSystem()
    event = events.Event("runid_es4", root)
    eventSystem.publishEvent(topicName, event)

if __name__ == "__main__":
    host = "lsst8.ncsa.uiuc.edu"
    topic = "test_events_3"

    eventSystem = events.EventSystem.getDefaultEventSystem()
    eventSystem.createTransmitter(host, topic)
    eventSystem.createReceiver(host, topic)

    #
    # send a test event, and wait to receive it
    #
    sendEvent(topic)

    val = eventSystem.receiveEvent(topic)
    assert val != None


    #
    # wait a short time to receive an event.  none was sent, so we should
    # time out and confirm that we didn't get anything
    #
    val = eventSystem.receiveEvent(topic,100)
    assert val == None
