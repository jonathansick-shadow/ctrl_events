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
        host_pid = "%s_%d" % (host,pid)
        self.broker_host = "lsst8.ncsa.illinois.edu"
        self.broker_port = 61616
        self.topic = events.LogEvent.LOGGING_TOPIC+"_"+host_pid

    def tearDown(self):
        """Remove the temporary directory."""
        shutil.rmtree(self.tempDir)

    def configure(self, configuration):
        """
        Create a configuration file in the temporary directory and populate
        it with the provided string.
        """
        log.configure_prop(configuration.format(self.outputFilename))

    def verify(self, ev, level, message):
        props = ev.getPropertySet()
        filename = os.path.basename(__file__)
        self.assertEqual(props.get(events.LogEvent.LOGGER), "component")
        self.assertEqual(props.get(events.LogEvent.LEVEL), level)
        self.assertEqual(props.get(events.LogEvent.MESSAGE), message)
        loc = props.get(events.LogEvent.LOCATION)
        self.assertEqual(loc.get(events.LogEvent.FILENAME), filename)


###############################################################################

    def testEventAppender(self):
        """Test configuring logging to go to a file."""

        recv = events.EventReceiver("lsst8.ncsa.illinois.edu", self.topic)

        self.configure("""
log4j.rootLogger=TRACE, EA
log4j.appender.EA=EventAppender
log4j.appender.EA.broker_host="""+self.broker_host+"""
log4j.appender.EA.logging_topic="""+self.topic+"""
""")

        msg1 = "This is TRACE"
        msg2 = "This is INFO"
        msg3 = "This is DEBUG"
        log.MDC("x", 3)
        with log.LogContext("component"):
            log.trace(msg1)
            log.info(msg2)
            log.debug(msg3)
        log.MDCRemove("x")

        filename = os.path.basename(__file__)

        ev1 = recv.receiveEvent()
        self.verify(ev1, log.TRACE, msg1)

        ev2 = recv.receiveEvent()
        self.verify(ev2, log.INFO, msg2)

        ev3 = recv.receiveEvent()
        self.verify(ev3, log.DEBUG, msg3)

####################################################################################

    def testEventAppenderOptions(self):
        """Test configuring logging to go to a file."""


        recv = events.EventReceiver("lsst8.ncsa.illinois.edu", self.topic)

        self.configure("""
log4j.rootLogger=TRACE, EA
log4j.appender.EA=EventAppender
log4j.appender.EA.broker_host="""+self.broker_host+"""
log4j.appender.EA.broker_port="""+str(self.broker_port)+"""
log4j.appender.EA.logging_topic="""+self.topic+"""
""")
        msg = "This is TRACE"
        log.MDC("x", 3)
        with log.LogContext("component"):
            log.trace(msg)
        log.MDCRemove("x")

        filename = os.path.basename(__file__)

        ev1 = recv.receiveEvent()
        self.verify(ev1, log.TRACE, msg)

####################################################################################

def main():
    unittest.main()

if __name__ == "__main__":
    main()
