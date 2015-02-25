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

#
# EventListener - This utility prints out events, given a host name and 
#                 topic.  This allows you to keep an eye on events while
#                 they are happening for debugging.
#
#
import lsst.ctrl.events as events
import lsst.pex.logging as logging
import time
import sys

def printValue(log, info, dp):
    logRec = logging.LogRec(log, info)
    pValue(logRec,  dp)
    logRec << logging.endr;

def pValue(logRec, dp):
        logRec << dp

if __name__ == "__main__":
    print "starting...\n"
    host = sys.argv[1]
    topic = sys.argv[2]
    print host+"->"+topic
    x = events.EventReceiver(host, topic)

    logger = logging.ScreenLog(1)
    tlog = logging.Log(logger, "test")

    print "waiting on receive...\n"
    while (True):
        val = x.receive()
        if val != None:
            printValue(tlog, logging.Log.INFO, val)

