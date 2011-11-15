// -*- lsst-c++ -*-

/* 
 * LSST Data Management System
 * Copyright 2008, 2009, 2010 LSST Corporation.
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
 * see <http://www.lsstcorp.org/LegalNotices/>.
 */
 
/** \file CommandEvent.cc
  *
  * \brief Command Event implementation
  *
  * \ingroup ctrl/events
  *
  * \author Stephen R. Pietrowicz, NCSA
  *
  */
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <cstring>

#include "lsst/ctrl/events/EventTypes.h"
#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/CommandEvent.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/daf/base/DateTime.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/exceptions.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <time.h>

#include <activemq/core/ActiveMQConnectionFactory.h>


using namespace std;

namespace lsst {
namespace ctrl {
namespace events {

const std::string CommandEvent::ORIGINATORID = "ORIGINATORID";
const std::string CommandEvent::ORIG_LOCALID = "ORIG_LOCALID";
const std::string CommandEvent::ORIG_PROCESSID = "ORIG_PROCESSID";
const std::string CommandEvent::ORIG_IPID = "ORIG_IPID";

const std::string CommandEvent::DESTINATIONID = "DESTINATIONID";
const std::string CommandEvent::DEST_LOCALID = "DEST_LOCALID";
const std::string CommandEvent::DEST_PROCESSID = "DEST_PROCESSID";
const std::string CommandEvent::DEST_IPID = "DEST_IPID";

/** \brief Creates CommandEvent which contains a PropertySet
  *
  */

CommandEvent::CommandEvent() : Event() {
    _init();
}


void CommandEvent::_init() {
    _keywords.insert(ORIGINATORID);
    _keywords.insert(ORIG_LOCALID);
    _keywords.insert(ORIG_PROCESSID);
    _keywords.insert(ORIG_IPID);

    _keywords.insert(DESTINATIONID);
    _keywords.insert(DEST_LOCALID);
    _keywords.insert(DEST_PROCESSID);
    _keywords.insert(DEST_IPID);
}

CommandEvent::CommandEvent(cms::TextMessage *msg) : Event(msg) {
    _init();


    _psp->set(ORIGINATORID, (int64_t)msg->getLongProperty(ORIGINATORID));
    _psp->set(ORIG_LOCALID, (short)msg->getShortProperty(ORIG_LOCALID));
    _psp->set(ORIG_PROCESSID, (short)msg->getShortProperty(ORIG_PROCESSID));
    _psp->set(ORIG_IPID, (int)msg->getIntProperty(ORIG_IPID));

    _psp->set(DESTINATIONID, (int64_t)msg->getLongProperty(DESTINATIONID));
    _psp->set(DEST_LOCALID, (short)msg->getShortProperty(DEST_LOCALID));
    _psp->set(DEST_PROCESSID, (short)msg->getShortProperty(DEST_PROCESSID));
    _psp->set(DEST_IPID, (int)msg->getIntProperty(DEST_IPID));

}

CommandEvent::CommandEvent( const std::string& runId, const int64_t originatorId, const int64_t destinationId, const PropertySet::Ptr psp) : Event(runId, *psp) {
    _constructor(runId, originatorId, destinationId, *psp);
}

CommandEvent::CommandEvent( const std::string& runId, const int64_t originatorId, const int64_t destinationId, const PropertySet& ps) : Event(runId, ps) {
    _constructor(runId, originatorId, destinationId, ps);
}

void CommandEvent::_constructor( const std::string& runId, const int64_t originatorId, const int64_t destinationId, const PropertySet& ps) {
    _init();

    EventSystem eventSystem = EventSystem().getDefaultEventSystem();

    //_originatorId = eventSystem.createOriginatorId();
    _psp->set(ORIGINATORID, originatorId);
    _psp->set(ORIG_LOCALID, eventSystem.extractLocalId(originatorId));
    _psp->set(ORIG_PROCESSID, eventSystem.extractProcessId(originatorId));
    _psp->set(ORIG_IPID, eventSystem.extractIPId(originatorId));

    _psp->set(DESTINATIONID, destinationId);
    _psp->set(DEST_LOCALID, eventSystem.extractLocalId(destinationId));
    _psp->set(DEST_PROCESSID, eventSystem.extractProcessId(destinationId));
    _psp->set(DEST_IPID, eventSystem.extractIPId(destinationId));

    _psp->set(TYPE, EventTypes::COMMAND);

}

void CommandEvent::populateHeader(cms::TextMessage* msg) const {
    Event::populateHeader(msg);

    msg->setLongProperty(ORIGINATORID, _psp->get<int64_t>(ORIGINATORID));
    msg->setShortProperty(ORIG_LOCALID, _psp->get<short>(ORIG_LOCALID));
    msg->setShortProperty(ORIG_PROCESSID, _psp->get<short>(ORIG_PROCESSID));
    msg->setIntProperty(ORIG_IPID, _psp->get<int>(ORIG_IPID));

    msg->setLongProperty(DESTINATIONID, _psp->get<int64_t>(DESTINATIONID));
    msg->setShortProperty(DEST_LOCALID, _psp->get<short>(DEST_LOCALID));
    msg->setShortProperty(DEST_PROCESSID, _psp->get<short>(DEST_PROCESSID));
    msg->setIntProperty(DEST_IPID, _psp->get<int>(DEST_IPID));
}

int64_t CommandEvent::getOriginatorId() { 
    return _psp->get<int64_t>(ORIGINATORID);
}

short CommandEvent::getOriginatorLocalId() { 
    return _psp->get<short>(ORIG_LOCALID);
}

short CommandEvent::getOriginatorProcessId() { 
    return _psp->get<short>(ORIG_PROCESSID);
}

int CommandEvent::getOriginatorIPId() {
     return _psp->get<int>(ORIG_IPID);
}

int64_t CommandEvent::getDestinationId() { 
    return _psp->get<int64_t>(DESTINATIONID); 
}

short CommandEvent::getDestinationLocalId() {
     return _psp->get<short>(DEST_LOCALID);
}

short CommandEvent::getDestinationProcessId() { 
    return _psp->get<short>(DEST_PROCESSID);
}

int CommandEvent::getDestinationIPId() {
     return _psp->get<int>(DEST_IPID); 
}

/** \brief destructor
  */
CommandEvent::~CommandEvent() {
}

}
}
}
