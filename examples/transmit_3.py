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
import lsst.daf.base as base
import lsst.pex.policy as policy

if __name__ == "__main__":
    p = policy.Policy.createPolicy("policies/activemq_policy.paf", 1)
    x = events.EventTransmitter(p)

    root = base.PropertySet()
    root.addInt("PID",200)
    root.add("text", "hello, world")
    root.addFloat("flo", 3.14)

    root.addInt("PID2",300)
    root.add("text2", "hi there")
    root.addFloat("flo2", 2.11)

    root.addInt("sub PID",3)
    root.add("sub text", "woof")
    root.addFloat("sub flo", 7.08)


    root.addInt("PID3",400)
    root.add("text3", "moo cow")
    root.addFloat("flo3", 1.87)

    print root.toString()
    x.publish(root)
