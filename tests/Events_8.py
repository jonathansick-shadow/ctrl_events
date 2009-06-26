#!/usr/bin/env python

import threading
import lsst.ctrl.events as events
import lsst.daf.base as base
import lsst.pex.policy as policy
import time


#
# sendEvent() - shoot an event to a host on a certain topic
#
def sendEvent(hostName, topicName, ps, id):
    header = base.PropertySet()
    header.addString("runid",id);
    trans = events.EventTransmitter(hostName, topicName, header)
    
    trans.publish(ps)

def createIntProperty(name, value):
    root = base.PropertySet()
    root.addInt(name, value)
    return root

def createStringProperty(name, value):
    root = base.PropertySet()
    root.add(name, value)
    return root

if __name__ == "__main__":
    p = policy.Policy()
    host = "lsst8.ncsa.uiuc.edu"
    topic = "test_events_5"
    recv = events.EventReceiver(host, topic, "runid = 'srp200'")
    recv2 = events.EventReceiver(host, topic)


    # Integer tests

    #
    # send two test events, first PID ==  400, then PID == 300, then PID == 200
    #
    sendEvent(host, topic, createIntProperty("PID", 400), "srp199")
    sendEvent(host, topic, createIntProperty("PID", 300), "srp200")
    sendEvent(host, topic, createIntProperty("PID", 200), "srp201")

    #
    # wait for the srp201 event
    #
    val = recv.receive()

    assert val != None

    #
    # shouldn't receive any others on this topic for this receiver
    #
    val = recv.receive(1000)
    assert val == None

    # recv2 should receive all three messages
    val = recv2.receive()
    assert val != None

    val = recv2.receive()
    assert val != None

    val = recv2.receive()
    assert val != None
