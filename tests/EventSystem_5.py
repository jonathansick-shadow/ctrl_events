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
import lsst.daf.base as base
import lsst.pex.policy as policy
import time
import os, platform

#
# sendEvent() - shoot an event to a host on a certain topic
#
class SimpleEventSystemTestCase(unittest.TestCase):
    def sendEvent(self, topicName, ps):
        eventSystem = events.EventSystem.getDefaultEventSystem()
        
        event = events.Event("runid_es5",ps)
        eventSystem.publishEvent(topicName, event)
    
    def createIntProperty(self, name, value):
        root = base.PropertySet()
        root.addInt(name, value)
        return root
    
    def testEventSystem(self):
        host = "lsst8.ncsa.illinois.edu"
        topic = "test_events_6_%s_%d" % (platform.node(), os.getpid())
        eventSystem = events.EventSystem.getDefaultEventSystem()
        eventSystem.createReceiver(host, topic)
        eventSystem.createTransmitter(host, topic)
    
    
        # Integer tests
    
        #
        # send two test events, first PID ==  300, then PID == 200
        #
        self.sendEvent(topic, self.createIntProperty("PID", 300))
        self.sendEvent(topic, self.createIntProperty("PID", 200))

if __name__ == "__main__":
    unittest.main()
