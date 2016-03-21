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
 * @file StatusEvent.cc
 *
 * @ingroup ctrl/events
 *
 * @brief Status Event implementation
 *
 */

#include "lsst/ctrl/events/StatusEvent.h"
#include "lsst/ctrl/events/EventTypes.h"

namespace lsst {
namespace ctrl {
namespace events {

std::string const StatusEvent::ORIG_HOSTNAME = "ORIG_HOSTNAME";
std::string const StatusEvent::ORIG_PROCESSID = "ORIG_PROCESSID";
std::string const StatusEvent::ORIG_LOCALID = "ORIG_LOCALID";

StatusEvent::StatusEvent() : Event() {
    _init();
}


void StatusEvent::_init() {
    _keywords.insert(ORIG_HOSTNAME);
    _keywords.insert(ORIG_PROCESSID);
    _keywords.insert(ORIG_LOCALID);
}

StatusEvent::StatusEvent(cms::TextMessage *msg) : Event(msg) {
    _init();

    _psp->set(ORIG_HOSTNAME, (std::string)msg->getStringProperty(ORIG_HOSTNAME));
    _psp->set(ORIG_PROCESSID, (int)msg->getIntProperty(ORIG_PROCESSID));
    _psp->set(ORIG_LOCALID, (int)msg->getIntProperty(ORIG_LOCALID));
}

StatusEvent::StatusEvent(LocationId const& originatorID, 
                         PropertySet const& ps) : Event(ps) {
    _constructor(originatorID);
}

StatusEvent::StatusEvent(LocationId const& originatorID,
                         PropertySet const& ps, 
                         PropertySet const& filterable) : Event(ps, filterable) {
    _constructor(originatorID);
}

StatusEvent::StatusEvent(std::string const& runID, 
                         LocationId const& originatorID, 
                         CONST_PTR(PropertySet) psp) : Event(runID, *psp) {
    _constructor(originatorID);
}

StatusEvent::StatusEvent(std::string const& runID, 
                         LocationId const& originatorID, 
                         CONST_PTR(PropertySet) psp, 
                         PropertySet const& filterable) : Event(runID, *psp, filterable) {
    _constructor(originatorID);
}

StatusEvent::StatusEvent(std::string const& runID, 
                         LocationId const& originatorID, 
                         PropertySet const& ps) : Event(runID, ps) {
    _constructor(originatorID);
}

StatusEvent::StatusEvent(std::string const& runID, 
                         LocationId const& originatorID,
                         PropertySet const& ps,
                         PropertySet const& filterable) : Event(runID, ps, filterable) {
    _constructor(originatorID);
}

void StatusEvent::_constructor(LocationId const& originatorID) {
    _init();

    _psp->set(ORIG_HOSTNAME, originatorID.getHostName());
    _psp->set(ORIG_PROCESSID, originatorID.getProcessID());
    _psp->set(ORIG_LOCALID, originatorID.getLocalID());
    _psp->set(TYPE, EventTypes::STATUS);

}

void StatusEvent::populateHeader(cms::TextMessage* msg) const {
    Event::populateHeader(msg);

    msg->setStringProperty(ORIG_HOSTNAME, _psp->get<std::string>(ORIG_HOSTNAME));
    msg->setIntProperty(ORIG_PROCESSID, _psp->get<int>(ORIG_PROCESSID));
    msg->setIntProperty(ORIG_LOCALID, _psp->get<int>(ORIG_LOCALID));
}

LocationId *StatusEvent::getOriginator() {
    std::string hostname = _psp->get<std::string>(ORIG_HOSTNAME);
    int pid = _psp->get<int>(ORIG_PROCESSID);
    int local = _psp->get<int>(ORIG_LOCALID);
    return new LocationId(hostname, pid, local);
}

StatusEvent::~StatusEvent() {
}

}}}
