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

import os
import platform
import lsst.ctrl.events as events
import lsst.daf.base as base
import lsst.pex.policy as policy


#
#
#
class EventTransmitterTestCase(unittest.TestCase):
    def sendEvent(self, broker, topicName, value):
        root = base.PropertySet()
        root.addInt("FOO", value)
        trans = events.EventTransmitter(broker, topicName)
        event = events.Event("myrunid", root)
        trans.publishEvent(event)
    
    def testEventTransmitter(self):
        broker = "lsst8.ncsa.illinois.edu"
    
        topic = "test_events_5_%s_%d" % (platform.node(), os.getpid())
        recv = events.EventReceiver(broker, topic)
    
    
        # Integer tests
    
        #
        # send two test events
        #
        self.sendEvent(broker, topic, 300)
        self.sendEvent(broker, topic, 200)

        val = recv.receiveEvent()
        self.assertNotEqual(val, None)

        # check to see we got 300 back
        ps = val.getPropertySet()
        self.assertEqual(ps.get("FOO"), 300)

        val = recv.receiveEvent()
        self.assertNotEqual(val, None)

        # check to see we got 200 back
        ps = val.getPropertySet()
        self.assertEqual(ps.get("FOO"), 200)

        # check to see no other events are waiting
        val = recv.receiveEvent(1)
        self.assertEqual(val, None)

if __name__ == "__main__":
    unittest.main()
