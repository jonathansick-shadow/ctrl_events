#!/usr/bin/env python

# 
# LSST Data Management System
#
# Copyright 2008-2014  AURA/LSST.
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
# see <https://www.lsstcorp.org/LegalNotices/>.
#

import unittest

import lsst.ctrl.events as events
from lsst.daf.base import PropertySet
import lsst.pex.exceptions
import lsst.pex.logging as logging
import lsst.pex.policy as policy
import os, platform

class EventSystemReceiveTestCase(unittest.TestCase):
    def testEventSystemReceive(self):
        host = "lsst8.ncsa.illinois.edu"
        eventSystem = events.EventSystem().getDefaultEventSystem()

        topic = "EventSystem_8_test_%s_%d" % (platform.node(), os.getpid())

        
        eventSystem.createReceiver(host, topic)
       
        eventSystem.createTransmitter(host, topic)

        root = PropertySet()
        root.set("test", 12)
        root.set("STATUS", "important stuff")
        event = events.Event("eventsystem_8", root)


        eventSystem.publishEvent(topic, event)

        receivedEvent = eventSystem.receiveEvent(topic)

        assert receivedEvent != None
        print "custom property names"
        print receivedEvent.getCustomPropertyNames()
        print "Custom PropertySet"
        ps = receivedEvent.getCustomPropertySet()
        print ps.toString()
        print
        print "filterable property names"
        print receivedEvent.getFilterablePropertyNames()

        print "PropertySet"
        ps = receivedEvent.getPropertySet()
        print ps.toString()


if __name__ == "__main__":
    unittest.main()

