#!/usr/bin/env python

import threading
import lsst.ctrl.events as events
from lsst.daf.base import PropertySet

#
# Send an event
#
def sendEvent(hostName, topicName):
    trans = events.EventTransmitter(hostName, topicName)
    
    root = PropertySet()
    root.set("DATE","2007-07-01T14:28:32.546012")
    root.setInt("PID",200)
    root.set("HOST","lsst8.ncsa.uiuc.edu")
    root.set("IP","141.142.220.44")
    root.set("EVNT","test")
    root.set("misc1","data 1")
    root.set("misc2","data 2")
    root.setFloat("float_value", 3.14)
    
    event = events.Event("test3_runid", root)
    trans.publishEvent(event)

if __name__ == "__main__":
    host = "lsst8.ncsa.uiuc.edu"
    topic = "test_events_3"
    y = events.EventReceiver(host, topic)

    #
    # send a test event, and wait to receive it
    #
    sendEvent(host, topic)

    val = y.receiveEvent()
    assert val != None

    ps = val.getPropertySet()
    print ps.toString()


    #
    # wait a short time to receive an event.  none was sent, so we should
    # time out and confirm that we didn't get anything
    #
    val = y.receiveEvent(100)
    assert val == None

