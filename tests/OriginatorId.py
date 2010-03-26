#!/usr/bin/env python

import os, socket, struct
import lsst.ctrl.events as events
import lsst.daf.base as base
import lsst.pex.exceptions
import lsst.pex.logging as logging
import lsst.pex.policy as policy


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

    myaddr = getHostAddr()

    originatorId = eventSystem.createOriginatorId()

    localId = eventSystem.extractLocalId(originatorId)
    assert localId == 0
    processId = eventSystem.extractProcessId(originatorId)
    assert processId == os.getpid()
    hostId = eventSystem.extractHostId(originatorId)
    assert hostId == myaddr

    originatorId2 = eventSystem.createOriginatorId()

    localId = eventSystem.extractLocalId(originatorId2)
    assert localId == 1
    processId = eventSystem.extractProcessId(originatorId2)
    assert processId == os.getpid()
    hostId = eventSystem.extractHostId(originatorId2)
    assert hostId == myaddr
