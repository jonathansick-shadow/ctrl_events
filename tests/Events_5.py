#!/usr/bin/env python

import threading
import lsst.events as events
import lsst.daf.data as datap;
import lsst.pex.policy as policy;
import time


#
# sendEvent() - shoot an event to a host on a certain topic
#
def sendEvent(hostName, topicName, name, value):
    trans = events.EventTransmitter(hostName, topicName)
    
    root = datap.SupportFactory.createPropertyNode("root");
    pid = datap.DataProperty(name,value)
    
    root.addProperty(pid);
    trans.publish("log", root)

if __name__ == "__main__":
    host = "lsst8.ncsa.uiuc.edu"
    topic = "test_events_6"
    recv = events.EventReceiver(host, topic)


    # Integer tests

    #
    # send two test events, first PID ==  300, then PID == 200
    #
    sendEvent(host, topic, "PID", 300)
    sendEvent(host, topic, "PID", 200)

    #
    # shouldn't receive anything
    # 
    val = recv.matchingReceive("PID", 400, 1000)
    assert val.get() == None

    #
    # wait for the 200 event, check that we received
    # the value we wanted
    #
    val = recv.matchingReceive("PID", 200, 1000)
    assert val.get() != None

    pid = val.findUnique("PID",1)
    assert pid != None
    assert pid.getValueInt() == 200

    #
    # wait for the 300 event, check that we received
    # the value we wanted
    #
    val = recv.matchingReceive("PID", 300, 1000)
    assert val.get() != None

    pid = val.findUnique("PID",1)
    assert pid != None
    assert pid.getValueInt() == 300

    # String tests

    #
    # send two test events, first GREET == "HI", then GREET == "HELLO"
    #

    sendEvent(host, topic, "GREET", "HELLO")
    sendEvent(host, topic, "GREET", "HI")

    #
    # wait for the "HI" event, check that we received
    # the value we wanted
    #
    val = recv.matchingReceive("GREET", "HI", 1000)
    assert val.get() != None

    pid = val.findUnique("GREET",1)
    assert pid != None
    assert pid.getValueString() == "HI"

    #
    # wait for the "HELLO" event, check that we received
    # the value we wanted
    #
    val = recv.matchingReceive("GREET", "HELLO", 1000)
    assert val.get() != None

    pid = val.findUnique("GREET",1)
    assert pid != None
    assert pid.getValueString() == "HELLO"
