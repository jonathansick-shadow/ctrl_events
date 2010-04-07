#!/usr/bin/env python

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

dbLogger.connect(user, password, "events")

receiver = events.EventReceiver(broker, events.EventLog.LOGGING_TOPIC)
while True:
   event = receiver.receiveEvent()
   dbLogger.insertRecord(event.getPropertySet())

dbLogger.disconnect()
