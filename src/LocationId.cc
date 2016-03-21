// -*- lsst-c++ -*-

/* 
 * LSST Data Management System
 * Copyright 2008-2015  AURA/LSST.
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

/** 
 * @file LocationId.cc
 *
 * @ingroup ctrl/events
 *
 * @brief host information
 *
 */

#include "lsst/ctrl/events/LocationId.h"
#include "lsst/ctrl/events/Host.h"

namespace lsst {
namespace ctrl {
namespace events {

LocationId::LocationId() {
    Host host = Host::getHost();
    _hostname = host.getHostName();
    _pid = getpid();
    _localID = _localCounter++;
}

LocationId::LocationId(std::string const& hostname, int pid, int localID) : 
    _hostname(hostname),
    _pid(pid),
    _localID(localID) 
    {}

LocationId::LocationId(LocationId const& id) {
    _hostname = id.getHostName();
    _pid = id.getProcessID();
    _localID = id.getLocalID();
}

int LocationId::_localCounter = 0;

std::string LocationId::getHostName() const {
    return _hostname;
}

int LocationId::getProcessID() const {
    return _pid;
}

int LocationId::getLocalID() const {
    return _localID;
}

}}}
