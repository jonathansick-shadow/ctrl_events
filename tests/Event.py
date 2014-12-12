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
from lsst.daf.base import PropertySet
from socket import gethostname

if __name__ == "__main__":
    runid = "testrunid"
    status = "my special status"
    root = PropertySet()
    MYNAME = "myname"
    root.set(MYNAME, MYNAME)
    root.set(events.Event.STATUS, status)
    
    event = events.Event(runid, root)

    # return the property set
    props = event.getPropertySet()
    print "PropertySet: "
    print props.toString()
    print
    assert props.get(events.Event.EVENTTIME) > 0
    assert props.get(events.Event.HOSTNAME) == gethostname()
    assert props.get(events.Event.PUBTIME) == 0
    assert props.get(events.Event.RUNID) == runid
    assert props.get(events.Event.STATUS) == status
    assert props.get(events.Event.TOPIC) == events.Event.UNINITIALIZED
    assert props.get(events.Event.TYPE) == events.EventTypes.EVENT
    assert props.get(MYNAME) == MYNAME

    filterableNames = event.getFilterablePropertyNames()
    print "Filterable names = ",filterableNames
    filterableNames.remove(events.Event.EVENTTIME)
    filterableNames.remove(events.Event.HOSTNAME)
    filterableNames.remove(events.Event.PUBTIME)
    filterableNames.remove(events.Event.RUNID)
    filterableNames.remove(events.Event.STATUS)
    filterableNames.remove(events.Event.TOPIC)
    filterableNames.remove(events.Event.TYPE)
    assert len(filterableNames) == 0

    customNames = event.getCustomPropertyNames()
    print "Custom names = ",customNames

    assert len(customNames) == 1
    assert customNames[0] == MYNAME

    eventTime = event.getEventTime()
    print "getEventTime() = ",event.getEventTime()
    print "getEventDate() = ",event.getEventDate()
    print "getPubTime() = ",event.getPubTime()
    assert event.getPubTime() == 0

    print "getHostId() = ", event.getHostId()

    assert event.getRunId() == runid
    assert event.getType() == events.EventTypes.EVENT
    assert event.getStatus() == status

    # check to be sure we really update the time
    event.updateEventTime()
    assert eventTime < event.getEventTime()

    # set the time to zero, and make sure we get back the same time.
    event.setEventTime(0)
    assert event.getEventTime() == 0

    # reset the time to eventTime, and make sure we get back the same time.
    event.setEventTime(eventTime)
    assert event.getEventTime() == eventTime


