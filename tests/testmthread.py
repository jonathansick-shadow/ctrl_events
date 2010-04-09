#!/usr/bin/env python
"""
Tests of the BlackboardItem classes
"""
from __future__ import with_statement

import pdb                              # we may want to say pdb.set_trace()
import os
import sys
import unittest
import time
from threading import Thread, Condition
from lsst.ctrl.events import EventSystem

eventBrokerHost = "lsst8.ncsa.uiuc.edu"
testtopic = "testmthread"

class Flag(object):
    def __init__(self):
        self.lock = Condition()
        self.markers = []

class EventThread(Thread):
    def __init__(self, flag):
        Thread.__init__(self, name="event")
        # self.setDaemon(True)
        self.flag = flag

    def run(self):
        with self.flag.lock:
            esys = EventSystem.getDefaultEventSystem()
            esys.createReceiver(eventBrokerHost, testtopic)
            # print "listening for", testtopic, "events"
            self.flag.markers.append("thread ready")
            self.flag.lock.notifyAll()

        # we will wait 1 second (5x what the main thread is sleeping)
        esys.receiveEvent(testtopic, 1000)
        # print "receive timed out"
        # sys.stdout.flush()
        self.flag.markers.append("timeout")
        

class MultiThreadTestCase(unittest.TestCase):
    def setUp(self):
        pass

    def tearDown(self):
        pass

    def testLockRelease(self):
        flag = Flag()
        t = EventThread(flag)

        # wait until the thread has started up
        with flag.lock:
            # print "starting thread"
            flag.markers.append("thread starting")
            t.start()
            flag.lock.wait()

        # by the time we wake up, the other thread should be listening
        # for an event (that will never come :-o)
        time.sleep(0.2)
        # print "waiting for receive"
        # sys.stdout.flush()
        flag.markers.append("waiting")
        t.join()

        print flag.markers
        self.assertEquals(flag.markers[2], "waiting",
                          "receive failed to release GIL")
    
__all__ = "MultiThreadTestCase".split()

if __name__ == "__main__":
    unittest.main()
