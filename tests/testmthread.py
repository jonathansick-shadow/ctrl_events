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

"""
Tests of the BlackboardItem classes
"""
from __future__ import with_statement

import pdb                              # we may want to say pdb.set_trace()
import os, platform
import sys
import unittest
import time
from threading import Thread, Condition
from lsst.ctrl.events import EventSystem
import lsst.utils.tests as tests
from testEnvironment import TestEnvironment

class Flag(object):
    def __init__(self):
        self.lock = Condition()
        self.markers = []

class EventThread(Thread):
    def __init__(self, broker, flag):
        Thread.__init__(self, name="event")
        # self.setDaemon(True)
        self.broker = broker
        self.flag = flag

    def run(self):
        testtopic = "testmthread_%s_%d" % (platform.node(), os.getpid())
        with self.flag.lock:
            esys = EventSystem.getDefaultEventSystem()
            esys.createReceiver(self.broker, testtopic)
            # print "listening for", testtopic, "events"
            self.flag.markers.append("thread ready")
            self.flag.lock.notifyAll()

        # we will wait 1 second (5x what the main thread is sleeping)
        esys.receiveEvent(testtopic, 1000)
        # print "receive timed out"
        # sys.stdout.flush()
        self.flag.markers.append("timeout")
        

class MultiThreadTestCase(unittest.TestCase):
    """Test multitreaded sends"""
    def setUp(self):
        pass

    def tearDown(self):
        pass

    @unittest.skipUnless(TestEnvironment().validTestDomain(), "not within valid domain")
    def testLockRelease(self):
        testEnv = TestEnvironment()
        broker = testEnv.getBroker()
        thisHost = platform.node()

        flag = Flag()
        t = EventThread(broker, flag)

        # wait until the thread has started up
        with flag.lock:
            # print "starting thread"
            flag.markers.append("thread starting")
            t.start()
            flag.lock.wait(2.0)

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


def suite():
    """Returns a suite containing all the tests cases in this module."""
    tests.init()
    suites = []
    suites += unittest.makeSuite(MultiThreadTestCase)
    suites += unittest.makeSuite(tests.MemoryTestCase)
    return unittest.TestSuite(suites)

def run(shouldExit=False):
    """Run the tests."""
    tests.run(suite(), shouldExit)

if __name__ == "__main__":
    run(True)
