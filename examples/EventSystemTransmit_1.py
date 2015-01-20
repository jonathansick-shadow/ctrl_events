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

import lsst.daf.base as base
import lsst.ctrl.events as events

if __name__ == "__main__":

    system = events.EventSystem().getDefaultEventSystem()
    system.createTransmitter("lsst8.ncsa.illinois.edu", "test1")
    system.createTransmitter("lsst8.ncsa.illinois.edu", "test2")
    system.createTransmitter("lsst8.ncsa.illinois.edu", "test3")

    newSystem = events.EventSystem().getDefaultEventSystem()

    root = base.PropertySet()
    root.add("DATE","2007-07-01T14:28:32.546012")
    root.addInt("PID",200)
    root.add("HOST","lsst8.ncsa.illinois.edu")
    root.add("IP","141.142.220.44")
    root.add("EVNT","test")
    root.add("misc1","data 1")
    root.add("misc2","data 2")
    root.addDouble("float_value", 3.14)

    newSystem.publishEvent("test1", events.Event(root))
    newSystem.publishEvent("test2", events.Event(root))
    newSystem.publishEvent("test3", events.Event(root))
