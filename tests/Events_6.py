#!/usr/bin/env python

import threading
import lsst.ctrl.events as events
import lsst.daf.base as base
import time


#
# Send an event
#
def sendEvent(hostName, topicName):
    trans = events.EventTransmitter(hostName, topicName)
    
    root = base.PropertySet()
    root.set("DATE","2007-07-01T14:28:32.546012")
    root.setInt("PID",200)
    root.set("HOST","lsst8.ncsa.uiuc.edu")
    root.set("IP","141.142.220.44")
    root.set("EVNT","test")
    root.set("misc1","data 1")
    root.set("misc2","data 2")
    root.setFloat("float_value", 3.14)
    
    trans.publish(root)

if __name__ == "__main__":
    host = "lsst8.ncsa.uiuc.edu"
    topic1 = "test_events_3"
    topic2 = "test_events_3a"
    print "1"
    y1 = events.EventReceiver(host, topic1)
    print "2"
    y2 = events.EventReceiver(host, topic2)

    print "3"
    #
    # send a test event on both topics at once, and have each receiver wait to
    # receive it
    #
    sendEvent(host, topic1+","+topic2)

    print "4"
    val = y1.receive()
    assert val != None
    print val.toString()

    print "5"
    val = y2.receive()
    assert val != None
    print val.toString()
    print "6"
