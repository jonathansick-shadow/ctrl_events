#!/usr/bin/env python

import os, socket, struct
import lsst.ctrl.events as events
import lsst.daf.base as base
import lsst.pex.exceptions
import lsst.pex.logging as logging
import lsst.pex.policy as policy
from lsst.daf.base import PropertySet


# return the ip address in an integer
def getHostAddr():
    
    hostname = socket.gethostname()
    (name, aliaslist, ipaddrlist) = socket.gethostbyname_ex(hostname)
    exp = 3
    addr = 0
    for byte in ipaddrlist[0].split('.'):
        addr = addr + (int(byte) * (256 ** exp))
        exp = exp -1
    return addr


if __name__ == "__main__":
    eventSystem = events.EventSystem().getDefaultEventSystem()

    originatorId = eventSystem.createOriginatorId()

    localId = eventSystem.extractLocalId(originatorId)
    assert localId == 0
    processId = eventSystem.extractProcessId(originatorId)
    assert processId == os.getpid()

    originatorId2 = eventSystem.createOriginatorId()

    localId = eventSystem.extractLocalId(originatorId2)
    assert localId == 1
    processId = eventSystem.extractProcessId(originatorId2)
    assert processId == os.getpid()

    IPId = eventSystem.extractIPId(originatorId2)


    root = PropertySet()
    root.set("myname","myname")
    status = "my special status"
    root.set(events.Event.STATUS, status)

    statusEvent = events.StatusEvent("my runid", originatorId2, root)

    transmitter = events.EventTransmitter("lsst8.ncsa.uiuc.edu", "mytopic")
    sel = "%s = %d" % (events.StatusEvent.IPID, IPId)
    #sel = "RUNID = '%s'" % "my runid"
    #sel = "PROCESSID = %d" % processId
    print sel
    receiver = events.EventReceiver("lsst8.ncsa.uiuc.edu", "mytopic", sel)

    transmitter.publishEvent(statusEvent)
    returnedEvent = receiver.receiveEvent()

    print "done"
    
