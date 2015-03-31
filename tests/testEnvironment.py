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

import platform

class TestEnvironment:
    """Information about this testing environment"""

    def __init__(self):

        # set these values to the broker host and port
        self.broker = "lsst8.ncsa.illinois.edu"
        self.port = 61616

        # add any domains where the tests will be running and still be
        # expected to reach the broker above.
        self.testDomains = ["ncsa.illinois.edu"]

    def getBroker(self):
        return self.broker

    def getPort(self):
        return self.port
        
    def getTestDomains(self):
        return self.testDomains

    def validTestDomain(self):
        """is the host this code is running on in the test domain list"""
        host = platform.node()        
        domain = host.split('.',1)[1]
        return domain in self.testDomains
