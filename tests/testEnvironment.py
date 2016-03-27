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

import os
import platform
import lsst.ctrl.events as events


class TestEnvironment:
    """Information about this testing environment"""

    def __init__(self):

        # set these values to the broker host and port
        self.broker = "lsst8.ncsa.illinois.edu"
        self.port = 61616
        logEvent = events.LogEvent()
        self.logging_topic = logEvent.getLoggingTopic()

        # add any domains where the tests will be running and still be
        # expected to reach the broker above.
        self.testDomains = ["ncsa.illinois.edu"]

        # add any hosts not in the domain above where the tests
        # will be running and still be expected to reach the broker.
        # this is here for anyone that wants to run these tests locally
        self.testHosts = []

    def getBroker(self):
        return self.broker

    def getPort(self):
        return self.port

    def getLoggingTopic(self):
        thisHost = platform.node()

        host_pid = "%s_%d" % (thisHost, os.getpid())

        topic = "%s_%s" % (self.logging_topic, host_pid)
        return topic

    def getTestDomains(self):
        return self.testDomains

    def validTestDomain(self):
        """is the host this code is running on in the test domain list"""
        host = platform.node()
        # no host?
        if host is None:
            return False
        # check to see if the host is listed as able to run the tests
        if host in self.testHosts:
            return True
        # if we don't have a domain at this point, and it's not in the
        # approved host list, return False
        if host.find('.') == -1:
            return False
        # split the domain portion off to see if it's in the valid domain list
        domain = host.split('.', 1)[1]
        return domain in self.testDomains
