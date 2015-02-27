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
from socket import gethostname

class EventTestCase(unittest.TestCase):
    """A test case for Event."""

    def testEventEmpty(self):
        status = "my special status"
        event = events.Event()
        props = event.getPropertySet() 
        self.assertEqual(props.nameCount(), 5)


    def testEventProperties(self):
        status = "my special status"
        root = PropertySet()
        MYNAME = "myname"
        root.set(MYNAME, MYNAME)
        root.set(events.Event.STATUS, status)
        
        event = events.Event(root)
        props = event.getPropertySet() 
        custom = event.getCustomPropertySet()

        self.assertEqual(props.nameCount(), 6)
        self.assertEqual(custom.nameCount(), 1)
        
        
    def testEventRunID(self):
        runid = "testrunid"
        status = "my special status"
        root = PropertySet()
        MYNAME = "myname"
        root.set(MYNAME, MYNAME)
        root.set(events.Event.STATUS, status)
        
        event = events.Event(runid, root)
    
        # return the property set
        props = event.getPropertySet()
        self.assertGreater(props.get(events.Event.EVENTTIME), 0)
        self.assertEqual(props.get(events.Event.PUBTIME), 0)
        self.assertEqual(props.get(events.Event.RUNID), runid)
        self.assertEqual(props.get(events.Event.STATUS), status)
        self.assertEqual(props.get(events.Event.TOPIC), events.Event.UNINITIALIZED)
        self.assertEqual(props.get(events.Event.TYPE), events.EventTypes.EVENT)
        self.assertEqual(props.get(MYNAME), MYNAME)
    
    
        filterableNames = event.getFilterablePropertyNames()
        filterableNames.remove(events.Event.EVENTTIME)
        filterableNames.remove(events.Event.PUBTIME)
        filterableNames.remove(events.Event.RUNID)
        filterableNames.remove(events.Event.STATUS)
        filterableNames.remove(events.Event.TOPIC)
        filterableNames.remove(events.Event.TYPE)
        assert len(filterableNames) == 0
    
        customNames = event.getCustomPropertyNames()
        self.assertEqual(len(customNames), 1)
        self.assertEqual(customNames[0], MYNAME)
    
        eventTime = event.getEventTime()
        self.assertEqual(event.getPubTime(),0)
    
        self.assertEqual(event.getRunId(), runid)
        self.assertEqual(event.getType(), events.EventTypes.EVENT)
        self.assertEqual(event.getStatus(), status)
    
        # check to be sure we really update the time
        event.updateEventTime()
        self.assertLess(eventTime, event.getEventTime())
    
        # set the time to zero, and make sure we get back the same time.
        event.setEventTime(0)
        self.assertEqual(event.getEventTime(), 0)
    
        # reset the time to eventTime, and make sure we get back the same time.
        event.setEventTime(eventTime)
        self.assertEqual(event.getEventTime(), eventTime)


if __name__ == "__main__":
    unittest.main()
    
