#!/usr/bin/env python

# 
# LSST Data Management System
# Copyright 2008, 2009, 2010 LSST Corporation.
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
# see <http://www.lsstcorp.org/LegalNotices/>.
#


import os
import sys
import subprocess
import lsst.ctrl.events as events

from lsst.ctrl.events.logging.DatabaseLogger import DatabaseLogger
from lsst.daf.persistence import DbAuth
from lsst.pex.policy import Policy


if len(sys.argv) > 1:
    broker = sys.argv[1]
    if broker.find('.') < 0:
        broker += ".ncsa.uiuc.edu"
if len(sys.argv) > 2:
    host = sys.argv[2]
if len(sys.argv) > 3:
    port = sys.argv[3]


home = os.getenv("HOME")
pol = Policy(home+"/.lsst/db-auth.paf")

dbAuth = DbAuth()
dbAuth.setPolicy(pol)

user = dbAuth.username(host,port)
password = dbAuth.password(host,port)

dbLogger = DatabaseLogger(host, int(port))

dbLogger.connect(user, password, "logs")

receiver = events.EventReceiver(broker, events.EventLog.LOGGING_TOPIC)
while True:
   event = receiver.receiveEvent()
   dbLogger.insertRecord("loggerdc3b", event.getPropertySet())

dbLogger.disconnect()
