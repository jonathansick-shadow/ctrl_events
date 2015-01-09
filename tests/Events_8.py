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

import unittest

import os
import platform
import lsst.ctrl.events as events
from lsst.daf.base import PropertySet

#
# Send an event
#

class SendValidEventTestCase(unittest.TestCase):
    def sendEvent(self, broker, topic):
        trans = events.EventTransmitter(broker, topic)
        
        root = PropertySet()
        root.set("TOPIC",topic)
        root.set("myname","myname")
        root.set("STATUS", "my special status")
        root.set("RUNID","srptestrun")
        
        event = events.Event("srptestrun", root)
        trans.publishEvent(event)

    def testSendEvent(self):
        broker = "lsst8.ncsa.illinois.edu"
    
        host = platform.node()
        pid = os.getpid()
        host_pid = "%s_%d" % (host, pid)
        topicA = "test_events_8_%s.A" % host_pid
        topicB = "test_events_8_%s.*" % host_pid
    
        yC = events.EventReceiver(broker, topicB)
    
        #
        # send a test event, and wait to receive it
        #
        self.sendEvent(broker, topicA)
    
        val = yC.receiveEvent()
        self.assertNotEqual(val, None)
        ps = val.getPropertySet()
        print ps.toString()
    
        print "+++++"
        retTopic = val.getTopic()
        print "topic = ",retTopic
        retStatus = val.getStatus()
        print "status = ",retStatus
        retRunId = val.getRunId()
        print "runId = ", retRunId

if __name__ == "__main__":
    unittest.main()
