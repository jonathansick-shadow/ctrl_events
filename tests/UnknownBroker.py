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
import lsst.daf.base as datap
import lsst.pex.exceptions as ex
import os, platform
import lsst.utils.tests as tests

class UnknownBrokerTestCase(unittest.TestCase):
    def testUnknownBroker(self):
        broker = "unknown_broker.ncsa.illinois.edu"
        topic = "test_events_7_%s_%d" % (platform.node(), os.getpid())
    
        # Test to be sure that a bad broker name throws a runtime event
        try:
            trans = events.EventTransmitter(broker, topic)
            self.assertEqual(0, 1)
        except ex.Exception as e:
            pass
    

def suite():
    """Returns a suite containing all the tests cases in this module."""
    tests.init()
    suites = []
    suites += unittest.makeSuite(UnknownBrokerTestCase)
    suites += unittest.makeSuite(tests.MemoryTestCase)
    return unittest.TestSuite(suites)

def run(shouldExit=False):
    """Run the tests."""
    tests.run(suite(), shouldExit)

if __name__ == "__main__":
    run(True)
