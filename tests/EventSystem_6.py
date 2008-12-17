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
    root.set("HOST","fester.ncsa.uiuc.edu")
    root.set("IP","141.142.220.44")
    root.set("EVNT","test")
    root.set("misc1","data 1")
    root.set("misc2","data 2")
    root.setFloat("float_value", 3.14)
    
    eventSystem = events.EventSystem.getDefaultEventSystem()
    eventSystem.publish(topicName, root)

if __name__ == "__main__":
    host = "lsst8.ncsa.uiuc.edu"
    topic1 = "test_events_3"
    topic2 = "test_events_3a"
    combinedTopic = topic1+","+topic2
    eventSystem = events.EventSystem.getDefaultEventSystem()
    eventSystem.createReceiver(host, topic1)
    eventSystem.createReceiver(host, topic2)
    eventSystem.createTransmitter(host, combinedTopic)

    #
    # send a test event on both topics at once, and have each receiver wait to
    # receive it
    #
    sendEvent(combinedTopic)

    val = eventSystem.receive(topic1)
    assert val != None
    print val.toString()

    val = eventSystem.receive(topic2)
    assert val != None
    print val.toString()

