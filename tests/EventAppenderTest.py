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
This tests the logging system in a variety of ways.
"""

import lsst.log as log
import os
import platform
import shutil
import sys
import tempfile
import unittest
import lsst.ctrl.events as events

class TestLog(unittest.TestCase):

    def setUp(self):
        """Make a temporary directory and a log file in it."""
        self.tempDir = tempfile.mkdtemp()
        self.outputFilename = os.path.join(self.tempDir, "log.out")
        self.stdout = None
        host = platform.node()
        pid = os.getpid()
        self.host_pid = "%s_%d" % (host,pid)

    def tearDown(self):
        """Remove the temporary directory."""
        shutil.rmtree(self.tempDir)

    def configure(self, configuration):
        """
        Create a configuration file in the temporary directory and populate
        it with the provided string.
        """
        log.configure_prop(configuration.format(self.outputFilename))

###############################################################################

    def testEventAppender(self):
        """Test configuring logging to go to a file."""

        topic = events.LogEvent.LOGGING_TOPIC+"_"+self.host_pid
        recv = events.EventReceiver("lsst8.ncsa.illinois.edu", topic)

        self.configure("""
log4j.rootLogger=TRACE, EA
log4j.appender.EA=EventAppender
log4j.appender.EA.broker_host=lsst8.ncsa.illinois.edu
log4j.appender.EA.logging_topic="""+events.LogEvent.LOGGING_TOPIC+"""_"""+self.host_pid+"""
""")

        log.MDC("x", 3)
        with log.LogContext("component"):
            log.trace("This is TRACE")
            log.info("This is INFO")
            log.debug("This is DEBUG")
        log.MDCRemove("x")
        print "here"

        filename = os.path.basename(__file__)

        ev1 = recv.receiveEvent()
        props = ev1.getPropertySet()
        print props.toString()
        self.assertEqual(props.get(events.LogEvent.LOGGER), "component")
        self.assertEqual(props.get(events.LogEvent.LEVEL), log.TRACE)
        self.assertEqual(props.get(events.LogEvent.MESSAGE), "This is TRACE")
        loc = props.get(events.LogEvent.LOCATION)
        self.assertEqual(loc.get(events.LogEvent.FILENAME), filename)

        ev2 = recv.receiveEvent()
        props = ev2.getPropertySet()
        self.assertEqual(props.get(events.LogEvent.LOGGER), "component")
        self.assertEqual(props.get(events.LogEvent.LEVEL), log.INFO)
        self.assertEqual(props.get(events.LogEvent.MESSAGE), "This is INFO")

        ev3 = recv.receiveEvent()
        props = ev3.getPropertySet()
        self.assertEqual(props.get(events.LogEvent.LOGGER), "component")
        self.assertEqual(props.get(events.LogEvent.LEVEL), log.DEBUG)
        self.assertEqual(props.get(events.LogEvent.MESSAGE), "This is DEBUG")

####################################################################################

    def testEventAppenderOptions(self):
        """Test configuring logging to go to a file."""


        topic = events.LogEvent.LOGGING_TOPIC+"_"+self.host_pid
        recv = events.EventReceiver("lsst8.ncsa.illinois.edu", topic)

        self.configure("""
log4j.rootLogger=TRACE, EA
log4j.appender.EA=EventAppender
log4j.appender.EA.broker_host=lsst8.ncsa.illinois.edu
log4j.appender.EA.broker_port=61616
log4j.appender.EA.logging_topic="""+events.LogEvent.LOGGING_TOPIC+"""_"""+self.host_pid+"""
""")
        log.MDC("x", 3)
        with log.LogContext("component"):
            log.trace("This is TRACE")
        log.MDCRemove("x")
        print "here"

        filename = os.path.basename(__file__)

        ev1 = recv.receiveEvent()
        props = ev1.getPropertySet()
        print props.toString()
        self.assertEqual(props.get(events.LogEvent.LOGGER), "component")
        self.assertEqual(props.get(events.LogEvent.LEVEL), log.TRACE)
        self.assertEqual(props.get(events.LogEvent.MESSAGE), "This is TRACE")
        loc = props.get(events.LogEvent.LOCATION)
        self.assertEqual(loc.get(events.LogEvent.FILENAME), filename)

####################################################################################

def main():
    unittest.main()

if __name__ == "__main__":
    main()
