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

#3
import unittest

import lsst.ctrl.events as events
import lsst.daf.base as base
import lsst.pex.exceptions
import lsst.pex.logging as logging
import os, platform

class EventSystemTestCase(unittest.TestCase):

    def testEventSystem(self):
        host = "lsst8.ncsa.illinois.edu"
        eventSystem = events.EventSystem.getDefaultEventSystem()
    
        topic = "EventSystem_test_%s_%d" % (platform.node(), os.getpid())
    
    
        eventSystem.createTransmitter(host, topic)
    
        root = base.PropertySet()
        root.addInt("test", 12)
    
        event = events.Event("runid_es3", root)
        eventSystem.publishEvent(topic, event)
    
        eventSystem.createReceiver(host, topic)

        rec = logging.LogRecord(-1,10)
        rec.addComment("a comment")
        event = events.LogEvent("runid_es3_log", rec)
        eventSystem.publishEvent(topic, event)

        val = eventSystem.receiveEvent(topic)
        ps = val.getPropertySet()

        names = [ "COMMENT", "DATE", "EVENTTIME", "LEVEL", "LOG", "PUBTIME", "RUNID", "STATUS", "TIMESTAMP", "TOPIC", "TYPE"]
        self.assertEqual(ps.nameCount(), len(names))
        for x in names:
            self.assertTrue(ps.exists(x))

if __name__ == "__main__":
    unittest.main()
