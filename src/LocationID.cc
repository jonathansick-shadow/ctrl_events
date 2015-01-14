// -*- lsst-c++ -*-

/* 
 * LSST Data Management System
 * Copyright 2008-2014  AURA/LSST.
 * 
 * This product includes software developed by the
 * LSST Project (http://www.lsst.org/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the LSST License Statement and 
 * the GNU General Public License along with this program.  If not, 
 * see <https://www.lsstcorp.org/LegalNotices/>.
 */

/** \file Location.cc
  *
  * \brief host information
  *
  * \ingroup events
  *
  */

#include <sys/types.h>
#include <unistd.h>
#include "lsst/ctrl/events/LocationID.h"
#include "lsst/ctrl/events/Host.h"

namespace lsst {
namespace ctrl {
namespace events {

/** \brief LocationID object. This object represents the originating process
  *        of an event.  When created, this represents a unique ID.
  */
LocationID::LocationID() {
    Host host = Host().getHost();
    _hostname = host.getHostName();
    _pid = getpid();
    _localID = _localCounter++;
}

/** \brief LocationID object. This object represents the originating process
  *        of an event.  When created, this represents a reconstituted ID.
  */
LocationID::LocationID(const std::string& hostname, int pid, int localID) {
    _constructor(hostname, pid, localID);
}

LocationID::LocationID(const LocationID& id) {
    _constructor(id.getHostName(), id.getProcessID(), id.getLocalID());
}

void LocationID::_constructor(const std::string& hostname, int pid, int localID) {
    _hostname = hostname;
    _pid = pid;
    _localID = localID;
}

/** \brief destructor
  */
LocationID::~LocationID() {
}

int LocationID::_localCounter=0;
int LocationID::_localID=0;
int LocationID::_pid=0;
std::string LocationID::_hostname;

std::string LocationID::getHostName() const {
    return _hostname;
}

int LocationID::getProcessID() const {
    return _pid;
}

int LocationID::getLocalID() const {
    return _localID;
}

}
}
}
