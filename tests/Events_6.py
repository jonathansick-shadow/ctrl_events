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


import threading
import lsst.ctrl.events as events
import lsst.daf.base as base
import time


#
# Send an event
#
def sendEvent(hostName, topicName):
    trans = events.EventTransmitter(hostName, topicName)
    
    root = base.PropertySet()
    root.set("DATE","2007-07-01T14:28:32.546012")
    root.setInt("PID",200)
    root.set("HOST","lsst8.ncsa.illinois.edu")
    root.set("IP","141.142.220.44")
    root.set("EVNT","test")
    root.set("misc1","data 1")
    root.set("misc2","data 2")
    root.setFloat("float_value", 3.14)
    
    event = events.Event("runid_test6",root)
    trans.publishEvent(event)

if __name__ == "__main__":
    host = "lsst8.ncsa.illinois.edu"
    topic1 = "test_events_6"
    topic2 = "test_events_6a"
    print "1"
    y1 = events.EventReceiver(host, topic1)
    print "2"
    y2 = events.EventReceiver(host, topic2)

    print "3"
    #
    # send a test event on both topics at once, and have each receiver wait to
    # receive it
    #
    sendEvent(host, topic1+","+topic2)

    print "4"
    val = y1.receiveEvent()
    assert val != None
    ps = val.getPropertySet()
    print ps.toString()

    print "5"
    val = y2.receiveEvent()
    assert val != None
    ps = val.getPropertySet()
    print ps.toString()
    print "6"
