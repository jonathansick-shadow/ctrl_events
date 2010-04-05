#!/usr/bin/env python

import time
import threading
import lsst.ctrl.events as events
import lsst.pex.policy as policy
from lsst.daf.base import PropertySet

#
# Send an event
#
def sendEvent(hostName, topic):
    trans = events.EventTransmitter(hostName, topic)

    eventSystem = events.EventSystem.getDefaultEventSystem()

    originatorId = eventSystem.createOriginatorId()

    root = PropertySet()
    root.set("TOPIC",topic)
    root.set("myname","myname")
    root.set("STATUS", "my special status")
    
    event = events.StatusEvent("srptestrun", originatorId, root)

    print "creating STATUS EVENT:"
    printEvent(event)

    statusOriginatorId = event.getOriginatorId()

    commandOriginatorId = eventSystem.createOriginatorId()

    root2 = PropertySet()
    root2.set("TOPIC",topic)
    root2.set("myname","myname2")
    root2.set("STATUS", "my special status2")
    event = events.CommandEvent("srptestrun", commandOriginatorId, statusOriginatorId, root2)

    # wait a short time so we can see the difference between the time 
    # the event is created and the time it is published
    # ok...now publish it
    trans.publishEvent(event)

def printEvent(val):
    print "custom property names"
    print val.getCustomPropertyNames()
    print "Custom PropertySet"
    ps = val.getCustomPropertySet()
    print ps.toString()
    print
    print "filterable property names"
    print val.getFilterablePropertyNames()

    print "PropertySet"
    ps = val.getPropertySet()
    print ps.toString()

if __name__ == "__main__":
    host = "lsst8.ncsa.uiuc.edu"
    topicA = "PIPELINE.A"

    receiver = events.EventReceiver(host, topicA)

    #
    # send a test event, and wait to receive it
    #
    sendEvent(host, topicA)

    val = receiver.receiveEvent()
    assert val != None

    print "COMMAND EVENT:"
    printEvent(val)

    eventsystem = events.EventSystem.getDefaultEventSystem()
    commandEvent = eventsystem.castToCommandEvent(val)
    
    print "originatorId = ", commandEvent.getOriginatorId()
    print "orig_localId = ", commandEvent.getOriginatorLocalId()
    print "orig_processId = ", commandEvent.getOriginatorProcessId()
    print "orig_IPId = ", commandEvent.getOriginatorIPId()

    print "DestinationId = ", commandEvent.getDestinationId()
    print "dest_localId = ", commandEvent.getDestinationLocalId()
    print "dest_processId = ", commandEvent.getDestinationProcessId()
    print "dest_IPId = ", commandEvent.getDestinationIPId()
