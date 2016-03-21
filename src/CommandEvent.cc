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
 * @file CommandEvent.cc
 *
 * @ingroup ctrl/events
 *
 * @brief Command Event implementation
 *
 */

#include "lsst/ctrl/events/CommandEvent.h"
#include "lsst/ctrl/events/EventTypes.h"

using namespace std;

namespace lsst {
namespace ctrl {
namespace events {

const std::string CommandEvent::ORIG_HOSTNAME = "ORIG_HOSTNAME";
const std::string CommandEvent::ORIG_PROCESSID = "ORIG_PROCESSID";
const std::string CommandEvent::ORIG_LOCALID = "ORIG_LOCALID";

const std::string CommandEvent::DEST_HOSTNAME = "DEST_HOSTNAME";
const std::string CommandEvent::DEST_PROCESSID = "DEST_PROCESSID";
const std::string CommandEvent::DEST_LOCALID = "DEST_LOCALID";

CommandEvent::CommandEvent() : Event() {
    _init();
}


/** private method to initialize the CommandEvent
  */
void CommandEvent::_init() {
    _keywords.insert(ORIG_HOSTNAME);
    _keywords.insert(ORIG_PROCESSID);
    _keywords.insert(ORIG_LOCALID);

    _keywords.insert(DEST_HOSTNAME);
    _keywords.insert(DEST_PROCESSID);
    _keywords.insert(DEST_LOCALID);
}

CommandEvent::CommandEvent(cms::TextMessage *msg) : Event(msg) {
    _init();


    _psp->set(ORIG_HOSTNAME, (std::string)msg->getStringProperty(ORIG_HOSTNAME));
    _psp->set(ORIG_PROCESSID, (int)msg->getIntProperty(ORIG_PROCESSID));
    _psp->set(ORIG_LOCALID, (int)msg->getIntProperty(ORIG_LOCALID));

    _psp->set(DEST_HOSTNAME, (std::string)msg->getStringProperty(DEST_HOSTNAME));
    _psp->set(DEST_PROCESSID, (int)msg->getIntProperty(DEST_PROCESSID));
    _psp->set(DEST_LOCALID, (int)msg->getIntProperty(DEST_LOCALID));

}

CommandEvent::CommandEvent(LocationId const&  originator, LocationId const& destination, CONST_PTR(PropertySet)& psp) : Event(*psp) {
    _constructor(originator, destination);
}

CommandEvent::CommandEvent(LocationId const& originator, LocationId const&  destination, PropertySet const& ps) : Event(ps) {
    _constructor(originator, destination);
}

CommandEvent::CommandEvent(LocationId const&  originator, LocationId const&  destination, PropertySet const& ps, PropertySet const& filterable) : Event(ps, filterable) {
    _constructor(originator, destination);
}

CommandEvent::CommandEvent(std::string const& runId, LocationId const&  originator, LocationId const& destination, CONST_PTR(PropertySet)& psp) : Event(runId, *psp) {
    _constructor(originator, destination);
}

CommandEvent::CommandEvent( std::string const& runId, LocationId const&  originator, LocationId const&  destination, PropertySet const& ps) : Event(runId, ps) {
    _constructor(originator, destination);
}

CommandEvent::CommandEvent(std::string const& runId, LocationId const&  originator, LocationId const&  destination, PropertySet const& ps, PropertySet const& filterable) : Event(runId, ps, filterable) {
    _constructor(originator, destination);
}

/** private method common to all constructors containing, originator, the 
  * originating location of this event, and destination, the destination
  * location for this event.
  */
void CommandEvent::_constructor(LocationId const&  originator, LocationId const&  destination) {
    _init();

    _psp->set(ORIG_HOSTNAME, originator.getHostName());
    _psp->set(ORIG_PROCESSID, originator.getProcessID());
    _psp->set(ORIG_LOCALID, originator.getLocalID());

    _psp->set(DEST_HOSTNAME, destination.getHostName());
    _psp->set(DEST_PROCESSID, destination.getProcessID());
    _psp->set(DEST_LOCALID, destination.getLocalID());

    _psp->set(TYPE, EventTypes::COMMAND);

}

void CommandEvent::populateHeader(cms::TextMessage* msg) const {
    Event::populateHeader(msg);

    msg->setStringProperty(ORIG_HOSTNAME, _psp->get<std::string>(ORIG_HOSTNAME));
    msg->setIntProperty(ORIG_PROCESSID, _psp->get<int>(ORIG_PROCESSID));
    msg->setIntProperty(ORIG_LOCALID, _psp->get<int>(ORIG_LOCALID));

    msg->setStringProperty(DEST_HOSTNAME, _psp->get<std::string>(DEST_HOSTNAME));
    msg->setIntProperty(DEST_PROCESSID, _psp->get<int>(DEST_PROCESSID));
    msg->setIntProperty(DEST_LOCALID, _psp->get<int>(DEST_LOCALID));
}

PTR(LocationId) CommandEvent::getOriginator() const { 
    std::string hostname =  _psp->get<std::string>(ORIG_HOSTNAME);
    int pid =  _psp->get<int>(ORIG_PROCESSID);
    int local =  _psp->get<int>(ORIG_LOCALID);
    return PTR(LocationId)(new LocationId(hostname, pid, local));
}

PTR(LocationId) CommandEvent::getDestination() const { 
    std::string hostname = _psp->get<std::string>(DEST_HOSTNAME); 
    int pid = _psp->get<int>(DEST_PROCESSID); 
    int local = _psp->get<int>(DEST_LOCALID);
    return PTR(LocationId)(new LocationId(hostname, pid, local));
}

CommandEvent::~CommandEvent() {
}

}}}
