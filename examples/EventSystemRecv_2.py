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

import lsst.ctrl.events as events

if __name__ == "__main__":

    system = events.EventSystem.getDefaultEventSystem()
    system.createReceiver("lsst8.ncsa.uiuc.edu", "test1")
    system.createReceiver("lsst8.ncsa.uiuc.edu", "test2")
    system.createReceiver("lsst8.ncsa.uiuc.edu", "test3")

    dp1 = system.receive("test1", 5000)
    if dp1 == None:
        print "Nothing received from test1"
    else:
        print dp1.toString()


    dp2 = system.receive("test2", 5000)
    if dp2 == None:
        print "Nothing received from test2"
    else:
        print dp2.toString()

    dp3 = system.receive("test3", 5000)
    if dp3 == None:
        print "Nothing received from test3"
    else:
        print dp3.toString()

