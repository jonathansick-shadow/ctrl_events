#!/usr/bin/env python

import threading
import lsst.ctrl.events as events
from lsst.daf.base import PropertySet

#
# Send an event
#
def sendEvent(hostName, topic):
    trans = events.EventTransmitter(hostName, topic)
    
    root = PropertySet()
    root.set("TOPIC",topic)
    root.set("myname","myname")
    root.set("STATUS", "my special status")
    root.set("RUNID","srptestrun")
    
    event = events.Event("srptestrun", root)
    trans.publishEvent(event)

if __name__ == "__main__":
    host = "lsst8.ncsa.uiuc.edu"
    topicA = "PIPELINE.A"
    topicB = "PIPELINE.*"

    yC = events.EventReceiver(host, topicB)

    #
    # send a test event, and wait to receive it
    #
    sendEvent(host, topicA)

    val = yC.receiveEvent()
    assert val != None
    ps = val.getPropertySet()
    print ps.toString()

    print "+++++"
    retTopic = val.getTopic()
    print "topic = ",retTopic
    retStatus = val.getStatus()
    print "status = ",retStatus
    retRunId = val.getRunId()
    print "runId = ", retRunId
