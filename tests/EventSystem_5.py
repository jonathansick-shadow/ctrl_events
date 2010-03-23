#!/usr/bin/env python

import threading
import lsst.ctrl.events as events
import lsst.daf.base as base
import lsst.pex.policy as policy
import time


#
# sendEvent() - shoot an event to a host on a certain topic
#
def sendEvent(topicName, ps):
    eventSystem = events.EventSystem.getDefaultEventSystem()
    
    eventSystem.publish(topic, ps)

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
    eventSystem = events.EventSystem.getDefaultEventSystem()
    eventSystem.createReceiver(host, topic)
    eventSystem.createTransmitter(host, topic)


    # Integer tests

    #
    # send two test events, first PID ==  300, then PID == 200
    #
    sendEvent(topic, createIntProperty("PID", 300))
    sendEvent(topic, createIntProperty("PID", 200))

