#!/usr/bin/env python

# 
# LSST Data Management System
# Copyright 2008, 2009, 2010 LSST Corporation.
# 
# This product includes software developed by the
# LSST Project (http://www.lsst.org/).
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the LSST License Statement and 
# the GNU General Public License along with this program.  If not, 
# see <http://www.lsstcorp.org/LegalNotices/>.
#


import os
import platform
import lsst.ctrl.events as events
from lsst.daf.base import PropertySet
from socket import gethostname

#
# Send an event
#
if __name__ == "__main__":

    broker = "lsst8.ncsa.illinois.edu"
    topic = "test_events_3_%s_%d" % (platform.node(), os.getpid())

    runId = "test3_runid"
    recv = events.EventReceiver(broker, topic)

    trans = events.EventTransmitter(broker, topic)
    
    root = PropertySet()

    DATE = "date"
    DATE_VAL = "2007-07-01T14:28:32.546012"
    root.set(DATE, DATE_VAL)

    BLANK = "blank"
    BLANK_VAL = ""
    root.set(BLANK, BLANK_VAL)

    PID = "pid"
    PID_VAL = os.getpid()
    root.setInt(PID, PID_VAL)

    HOST = "host"
    HOST_VAL = "lsst8.ncsa.illinois.edu"
    root.set(HOST, HOST_VAL)

    IP = "ip"
    IP_VAL = "141.142.220.44"
    root.set(IP, IP_VAL)

    EVNT = "evnt"
    EVNT_VAL = "test"
    root.set(EVNT, EVNT_VAL)

    MISC1 = "misc1"
    MISC1_VAL = "data 1"
    root.set(MISC1, MISC1_VAL)

    MISC2 = "misc2"
    MISC2_VAL = "data 2"
    root.set(MISC2, MISC2_VAL)

    MISC3 = "misc3"
    MISC3_VAL = ""
    root.set(MISC3, MISC3_VAL)

    DATA = "data"
    DATA_VAL = 3.14
    root.setDouble(DATA, DATA_VAL)
    
    event = events.Event(runId, root)
    trans.publishEvent(event)


    val = recv.receiveEvent()
    assert val != None

    ps = val.getPropertySet()
    print ps.toString()
    assert ps.get(DATE) == DATE_VAL
    assert ps.get(BLANK) == BLANK_VAL
    assert ps.get(PID) == PID_VAL
    assert ps.get(HOST) == HOST_VAL
    assert ps.get(IP) == IP_VAL
    assert ps.get(EVNT) == EVNT_VAL
    assert ps.get(MISC1) == MISC1_VAL
    assert ps.get(MISC2) == MISC2_VAL
    assert ps.get(MISC3) == MISC3_VAL
    assert ps.get(DATA) == DATA_VAL

    assert ps.get(events.Event.EVENTTIME) > 0
    assert ps.get(events.Event.PUBTIME) > 0
    assert ps.get(events.Event.RUNID) == runId
    assert ps.get(events.Event.STATUS) == "unknown"
    assert ps.get(events.Event.TOPIC) == topic
    assert ps.get(events.Event.TYPE) == events.EventTypes.EVENT
    
    #
    # wait a short time to receive an event.  none was sent, so we should
    # time out and confirm that we didn't get anything
    #
    val = recv.receiveEvent(1000)
    assert val == None
