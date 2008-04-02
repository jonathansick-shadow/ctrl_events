#!/usr/bin/env python

import lsst.ctrl.events as events
import lsst.daf.base as datap;
import lsst.pex.exceptions as ex
import lsst.pex.logging as logging
import lsst.pex.policy as policy;


if __name__ == "__main__":
    host = "lsst8.ncsa.uiuc.edu"
    eventSystem = events.EventSystem().getDefaultEventSystem()

    # can't just pass in an empty policy file, because it
    # expects a topicName and (hostName or useLocalSockets == true)
    p = policy.Policy()
    try:
        eventSystem.createTransmitter(p)
    except ex.LsstNotFound:
        pass

    # host wasn't specified...that's a no-no, since useLocalSockets is false
    p.set("topicName", "EventSystem_test")
    try:
        eventSystem.createTransmitter(p)
    except ex.LsstNotFound:
        pass

    # TODO:
    #
    # need to add a test where a transmitter is created when
    # useLocalSockets is true, but currently you can't do that
    # because adding booleans to Policy doesn't work (Trac #258)

    topic = "EventSystem_1_test"

    eventSystem.createTransmitter(host, topic)

    root = datap.DataProperty.createPropertyNode("root")
    test = datap.DataProperty("test", 12)
    root.addProperty(test)

    eventSystem.publish(topic, root)

    # 
    # TODO: fix this logging transmission and reception
    #
    # rec = logging.LogRecord(-1,10)
    # rec.addComment("a comment")
    # eventSystem.publish(topic, rec)
