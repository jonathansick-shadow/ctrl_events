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
import threading
import lsst.ctrl.events as events
import lsst.daf.base as base
import lsst.pex.policy as policy


#
# sendEvent() - shoot an event to a host on a certain topic
#
def sendEvent(broker, topicName, ps):
    trans = events.EventTransmitter(broker, topicName)
    event = events.Event("myrunid",ps)
    trans.publishEvent(event)

def createIntProperty(name, value):
    root = base.PropertySet()
    root.addInt(name, value)
    return root

def createStringProperty(name, value):
    root = base.PropertySet()
    root.add(name, value)
    return root

if __name__ == "__main__":
    broker = "lsst8.ncsa.illinois.edu"

    topic = "test_events_5_%s_%d" % (platform.node(), os.getpid())
    recv = events.EventReceiver(broker, topic)


    # Integer tests

    #
    # send two test events, first PID ==  300, then PID == 200
    #
    sendEvent(broker, topic, createIntProperty("PID", 300))
    sendEvent(broker, topic, createIntProperty("PID", 200))
