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

    destinationId = eventSystem.createOriginatorId()

    root = PropertySet()
    root.set("myname","myname")
    status = "my special status"
    root.set(events.Event.STATUS, status)

    commandEvent = events.CommandEvent("my runid", originatorId, destinationId, root)

    transmitter = events.EventTransmitter("lsst8.ncsa.uiuc.edu", "mytopic")

    sel = "%s = %d" % (events.CommandEvent.DESTINATIONID, destinationId)
    receiver = events.EventReceiver("lsst8.ncsa.uiuc.edu", "mytopic", sel)

    transmitter.publishEvent(commandEvent)
    returnedEvent = receiver.receiveEvent(2000)

    assert returnedEvent != None

    print "done"
