#!/usr/bin/env python

import threading
import lsst.events as events
import lsst.mwi.data as datap;
import time


#
# Send an event
#
def sendEvent(hostName, topicName):
    trans = events.EventTransmitter(hostName, topicName)
    
    root = datap.SupportFactory.createPropertyNode("root");
    date = datap.DataProperty("DATE","2007-07-01T14:28:32.546012")
    pid = datap.DataProperty("PID",200)
    host = datap.DataProperty("HOST","fester.ncsa.uiuc.edu")
    ip = datap.DataProperty("IP","141.142.220.44")
    event = datap.DataProperty("EVNT","test")
    misc1 = datap.DataProperty("misc1","data 1")
    misc2 = datap.DataProperty("misc2","data 2")
    flo = datap.DataProperty("float_value", 3.14)
    
    root.addProperty(date);
    root.addProperty(pid);
    root.addProperty(host);
    root.addProperty(ip);
    root.addProperty(event);
    root.addProperty(misc1);
    root.addProperty(misc2);
    root.addProperty(flo);
    trans.publish("log", root)

if __name__ == "__main__":
    host = "lsst8.ncsa.uiuc.edu"
    topic1 = "test_events_3"
    topic2 = "test_events_3a"
    y1 = events.EventReceiver(host, topic1)
    y2 = events.EventReceiver(host, topic2)

    #
    # send a test event on both topics at once, and have each receiver wait to
    # receive it
    #
    sendEvent(host, topic1+","+topic2)

    val = y1.receive()
    assert val.get() != None
    print val.toString("-",1)

    val = y2.receive()
    assert val.get() != None
    print val.toString("=",1)
