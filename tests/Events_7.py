#!/usr/bin/env python

import threading
import lsst.events as events
import lsst.mwi.data as datap
import lsst.mwi.exceptions as ex
import time


if __name__ == "__main__":
    host = "unknown_host.ncsa.uiuc.edu"
    topic = "test_7_topic"

    # Test to be sure that a bad host name throws a runtime event
    try:
        trans = events.EventTransmitter(host, topic)
        assert 0
    except ex.LsstRuntime:
        assert 1
    
