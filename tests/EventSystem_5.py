#!/usr/bin/env python

import threading
import lsst.ctrl.events as events
import lsst.daf.base as datap;
import lsst.pex.policy as policy;
import time


#
# sendEvent() - shoot an event to a host on a certain topic
#
def sendEvent(topicName, name, value):
    eventSystem = events.EventSystem.getDefaultEventSystem()
    
    root = datap.DataProperty.createPropertyNode("root");
    pid = datap.DataProperty(name,value)
    
    root.addProperty(pid);
    eventSystem.publish(topic, root)

if __name__ == "__main__":
    host = "lsst8.ncsa.uiuc.edu"
    topic = "test_events_6"
    eventSystem = events.EventSystem.getDefaultEventSystem()
    eventSystem.createReceiver(host, topic)
    eventSystem.createTransmitter(host, topic)


    # Integer tests

    #
    # send two test events, first PID ==  300, then PID == 200
    #
    sendEvent(topic, "PID", 300)
    sendEvent(topic, "PID", 200)

    #
    # shouldn't receive anything
    # 
    val = eventSystem.matchingReceive(topic, "PID", 400, 1000)
    assert val.get() == None

    #
    # wait for the 200 event, check that we received
    # the value we wanted
    #
    val = eventSystem.matchingReceive(topic, "PID", 200, 1000)
    assert val.get() != None

    pid = val.findUnique("PID",1)
    assert pid != None
    assert pid.getValueInt() == 200

    #
    # wait for the 300 event, check that we received
    # the value we wanted
    #
    val = eventSystem.matchingReceive(topic, "PID", 300, 1000)
    assert val.get() != None

    pid = val.findUnique("PID",1)
    assert pid != None
    assert pid.getValueInt() == 300

    # String tests

    #
    # send two test events, first GREET == "HI", then GREET == "HELLO"
    #

    sendEvent(topic, "GREET", "HI")
    sendEvent(topic, "GREET", "HELLO")

    #
    # wait for the "HI" event, check that we received
    # the value we wanted
    #
    val = eventSystem.matchingReceive(topic, "GREET", "HI", 1000)
    assert val.get() != None

    pid = val.findUnique("GREET",1)
    assert pid != None
    assert pid.getValueString() == "HI"

    #
    # wait for the "HELLO" event, check that we received
    # the value we wanted
    #
    val = eventSystem.matchingReceive(topic, "GREET", "HELLO", 1000)
    assert val.get() != None

    pid = val.findUnique("GREET",1)
    assert pid != None
    assert pid.getValueString() == "HELLO"
