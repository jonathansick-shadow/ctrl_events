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
    host = "lsst8.ncsa.uiuc.edu"
    topic = "test_events_6"
    recv = events.EventReceiver(host, topic)


    # Integer tests

    #
    # send two test events, first PID ==  300, then PID == 200
    #
    sendEvent(host, topic, createIntProperty("PID", 300))
    sendEvent(host, topic, createIntProperty("PID", 200))
