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


import lsst.ctrl.events as events
import lsst.daf.base as base
import lsst.pex.exceptions
import lsst.pex.logging as logging
import lsst.pex.policy as policy


if __name__ == "__main__":
    host = "lsst8.ncsa.illinois.edu"
    eventSystem = events.EventSystem().getDefaultEventSystem()

    # can't just pass in an empty policy file, because it
    # expects a topicName and (hostName or useLocalSockets == true)
    p = policy.Policy()
    try:
        eventSystem.createTransmitter(p)
    except lsst.pex.exceptions.LsstCppException, e:
        pass

    # host wasn't specified...that's a no-no, since useLocalSockets is false
    p.set("topicName", "EventSystem_test")
    try:
        eventSystem.createTransmitter(p)
    except lsst.pex.exceptions.LsstCppException, e:
        pass

    # TODO:
    #
    # need to add a test where a transmitter is created when
    # useLocalSockets is true, but currently you can't do that
    # because adding booleans to Policy doesn't work (Trac #258)

    topic = "EventSystem_1_test"

    eventSystem.createTransmitter(host, topic)

    root = base.PropertySet()
    root.addInt("test", 12)

    event = events.Event("runid_es3", root)
    eventSystem.publishEvent(topic, event)

    # 
    # TODO: fix this logging transmission and reception
    #
    rec = logging.LogRecord(-1,10)
    rec.addComment("a comment")
    event = events.LogEvent("runid_es3_log", rec)
    eventSystem.publishEvent(topic, event)
