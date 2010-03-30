#!/usr/bin/env python

import threading
import lsst.ctrl.events as events
from lsst.daf.base import PropertySet

if __name__ == "__main__":
    root = PropertySet()
    root.set("myname","myname")
    # root.set("STATUS", "my special status")
    
    event = events.Event("srptestrun", root)

    props = event.getPropertySet()
    print "PropertySet: "
    print props.toString()
    print

    print "Filterable names = ",event.getFilterablePropertyNames()

    print "Custom names = ",event.getCustomPropertyNames()

    print "getEventTime() = ",event.getEventTime()
    print "getEventDate() = ",event.getEventDate()
    print "getPubTime() = ",event.getPubTime()
    print "getPubTime() = ",event.getPubDate()
