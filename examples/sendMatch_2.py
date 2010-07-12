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

import time
import lsst.ctrl.events as events
import lsst.daf.base as base

if __name__ == "__main__":
    x = events.EventTransmitter("lsst8.ncsa.uiuc.edu", "test")

    root0 = base.PropertySet()
    root0.addInt("PID",100)
    root0.add("EVNT","sent first; received third")
    x.publish(root0)

    time.sleep(6);

    root1 = base.PropertySet()
    root1.addInt("PID",400)
    root1.add("EVNT","sent second; received fourth")
    x.publish(root1)

    root2 = base.PropertySet()
    root2.addInt("PID",200)
    root2.add("EVNT","sent third; received first")
    x.publish(root2)

    root3 = base.PropertySet()
    root3.addInt("PID",500)
    root3.add("EVNT","sent fourth: received fifth")
    x.publish(root3)

    root4 = base.PropertySet()
    root4.addInt("PID",300)
    root4.add("EVNT","send fifth; received second")
    x.publish(root4)

