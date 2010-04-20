#!/usr/bin/env python

import threading
import lsst.ctrl.events as events
from lsst.daf.base import PropertySet

if __name__ == "__main__":
    runid = "srptestrun"
    status = "my special status"
    root = PropertySet()
    root.set("myname","myname")
    root.set(events.Event.STATUS, status)
    
    event = events.Event(runid, root)

    props = event.getPropertySet()
    print "PropertySet: "
    print props.toString()
    print

    filterableNames = event.getFilterablePropertyNames()
    print "Filterable names = ",filterableNames
    filterableNames.remove('EVENTTIME')
    filterableNames.remove('HOSTID')
    filterableNames.remove('PUBTIME')
    filterableNames.remove('RUNID')
    filterableNames.remove('STATUS')
    filterableNames.remove('TOPIC')
    filterableNames.remove('TYPE')
    assert len(filterableNames) == 0

    customNames = event.getCustomPropertyNames()
    print "Custom names = ",customNames

    assert len(customNames) == 1

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

    status2 = "hello there"
    event.setStatus(status2)
    assert event.getStatus() == status2

