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

    print "Filterable names = ",event.getFilterablePropertyNames()

    print "Custom names = ",event.getCustomPropertyNames()

    print "getEventTime() = ",event.getEventTime()
    print "getEventDate() = ",event.getEventDate()
    print "getPubTime() = ",event.getPubTime()
    assert event.getPubTime() == 0

    print "getHostId() = ", event.getHostId()

    assert event.getRunId() == runid
    assert event.getType() == events.EventTypes.EVENT
    assert event.getStatus() == status
