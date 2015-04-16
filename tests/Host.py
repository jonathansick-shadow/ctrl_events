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

import socket
import struct
import unittest
import lsst.ctrl.events as events
import lsst.utils.tests as tests

class HostTestCase(unittest.TestCase):
    """A test for Host"""

    def testHostIPAddress(self):
        host = events.Host.getHost()
        ipAddr1 = socket.inet_ntoa(struct.pack('!L',host.getIPAddress()))
        
        ipAddr2 = socket.gethostbyname(socket.gethostname())
        self.assertEqual(ipAddr1, ipAddr2)


    def testHostName(self):
        host = events.Host.getHost()
        self.assertEqual(socket.gethostname(), host.getHostName())
        
def suite():
    """Returns a suite containing all the tests cases in this module."""
    tests.init()
    suites = []
    suites += unittest.makeSuite(HostTestCase)
    suites += unittest.makeSuite(tests.MemoryTestCase)
    return unittest.TestSuite(suites)

def run(shouldExit=False):
    """Run the tests."""
    tests.run(suite(), shouldExit)

if __name__ == "__main__":
    run(True)
