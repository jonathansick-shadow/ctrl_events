#!/usr/bin/env python

import threading
import lsst.events as events
import lsst.daf.data as datap;
import lsst.pex.exceptions as ex;
import time


#
# Send an event
#
def sendEvent(topicName):
    
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
    eventSystem = events.EventSystem.getDefaultEventSystem()
    eventSystem.publish(topicName, root)

if __name__ == "__main__":
    topic1 = "test_events_3"
    eventSystem = events.EventSystem.getDefaultEventSystem()

    #
    # create a transmitter that has no receiver 
    # 
    try:
        eventSystem.createLocalTransmitter("moo")
    except ex.LsstRuntime:
        assert 1
    #
    # Create a local receiver and sender
    #
    eventSystem.createLocalReceiver(topic1)
    eventSystem.createLocalTransmitter(topic1)

    #
    # send a test event on both topics at once, and have each receiver wait to
    # receive it
    #
    sendEvent(topic1)

    val = eventSystem.receive(topic1)
    assert val.get() != None
