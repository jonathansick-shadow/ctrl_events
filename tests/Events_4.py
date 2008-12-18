#!/usr/bin/env python

import threading
import lsst.ctrl.events as events
import lsst.daf.base as base
import lsst.pex.policy as policy
import time


#
# sendEvent() - shoot an event to a host on a certain topic
#
def sendEvent(hostName, topicName, ps):
    trans = events.EventTransmitter(hostName, topicName)
    
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
    recv = events.EventReceiver(host, topic)


    # Integer tests

    #
    # send two test events, first PID ==  400, then PID == 300, then PID == 200
    #
    sendEvent(host, topic, createIntProperty("PID", 400))
    sendEvent(host, topic, createIntProperty("PID", 300))
    sendEvent(host, topic, createIntProperty("PID", 200))

    #
    # wait for the 200 event, check that we received
    # the value we wanted
    #
    val = recv.matchingReceive("PID", 200)

    pid = val.getAsInt("PID")
    assert pid == 200

    #
    # wait for the 300 event, check that we received
    # the value we wanted
    #
    val = recv.matchingReceive("PID", 300)

    pid = val.getAsInt("PID")
    assert pid == 300

    #
    # wait for the 400 event, and make sure we get it
    #
    val = recv.receive()
    pid = val.getAsInt("PID")
    assert pid == 400

    # String tests

    #
    # send two test events, first GREET == "HI", then GREET == "HELLO"
    #

    sendEvent(host, topic, createStringProperty("GREET", "HELLO"))
    sendEvent(host, topic, createStringProperty("GREET", "HI"))

    #
    # wait for the "HI" event, check that we received
    # the value we wanted
    #
    val = recv.matchingReceive("GREET", "HI")

    info = val.getString("GREET")
    assert info == "HI"

    #
    # wait for the "HELLO" event, check that we received
    # the value we wanted
    #
    val = recv.matchingReceive("GREET", "HELLO")

    info = val.getString("GREET")
    assert info == "HELLO"
