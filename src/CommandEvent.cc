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

#include "lsst/ctrl/events/LocationID.h"
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

const std::string CommandEvent::ORIG_IPID = "ORIG_IPID";
const std::string CommandEvent::ORIG_PROCESSID = "ORIG_PROCESSID";
const std::string CommandEvent::ORIG_LOCALID = "ORIG_LOCALID";

const std::string CommandEvent::DEST_IPID = "DEST_IPID";
const std::string CommandEvent::DEST_PROCESSID = "DEST_PROCESSID";
const std::string CommandEvent::DEST_LOCALID = "DEST_LOCALID";

/** \brief Creates CommandEvent which contains a PropertySet
  *
  */

CommandEvent::CommandEvent() : Event() {
    _init();
}


void CommandEvent::_init() {
    _keywords.insert(ORIG_IPID);
    _keywords.insert(ORIG_PROCESSID);
    _keywords.insert(ORIG_LOCALID);

    _keywords.insert(DEST_IPID);
    _keywords.insert(DEST_PROCESSID);
    _keywords.insert(DEST_LOCALID);
}

CommandEvent::CommandEvent(cms::TextMessage *msg) : Event(msg) {
    _init();


    _psp->set(ORIG_IPID, (int)msg->getIntProperty(ORIG_IPID));
    _psp->set(ORIG_PROCESSID, (int)msg->getIntProperty(ORIG_PROCESSID));
    _psp->set(ORIG_LOCALID, (int)msg->getIntProperty(ORIG_LOCALID));

    _psp->set(DEST_IPID, (int)msg->getIntProperty(DEST_IPID));
    _psp->set(DEST_PROCESSID, (int)msg->getIntProperty(DEST_PROCESSID));
    _psp->set(DEST_LOCALID, (int)msg->getIntProperty(DEST_LOCALID));

}

CommandEvent::CommandEvent( const std::string& runId, const LocationID&  originator, const LocationID& destination, const PropertySet::Ptr psp) : Event(runId, *psp) {
    _constructor(runId, originator, destination, *psp);
}

CommandEvent::CommandEvent( const std::string& runId, const LocationID&  originator, const LocationID&  destination, const PropertySet& ps) : Event(runId, ps) {
    _constructor(runId, originator, destination, ps);
}

void CommandEvent::_constructor( const std::string& runId, const LocationID&  originator, const LocationID&  destination, const PropertySet& ps) {
    _init();

    _psp->set(ORIG_IPID, originator.getIPAddress());
    _psp->set(ORIG_PROCESSID, originator.getProcessID());
    _psp->set(ORIG_LOCALID, originator.getLocalID());

    _psp->set(DEST_IPID, destination.getIPAddress());
    _psp->set(DEST_PROCESSID, destination.getProcessID());
    _psp->set(DEST_LOCALID, destination.getLocalID());

    _psp->set(TYPE, EventTypes::COMMAND);

}

void CommandEvent::populateHeader(cms::TextMessage* msg) const {
    Event::populateHeader(msg);

    msg->setIntProperty(ORIG_IPID, _psp->get<int>(ORIG_IPID));
    msg->setIntProperty(ORIG_PROCESSID, _psp->get<int>(ORIG_PROCESSID));
    msg->setIntProperty(ORIG_LOCALID, _psp->get<int>(ORIG_LOCALID));

    msg->setIntProperty(DEST_IPID, _psp->get<int>(DEST_IPID));
    msg->setIntProperty(DEST_PROCESSID, _psp->get<int>(DEST_PROCESSID));
    msg->setIntProperty(DEST_LOCALID, _psp->get<int>(DEST_LOCALID));
}

LocationID *CommandEvent::getOriginator() { 
    int ip =  _psp->get<int>(ORIG_IPID);
    int pid =  _psp->get<int>(ORIG_PROCESSID);
    int local =  _psp->get<int>(ORIG_LOCALID);
    return new LocationID(ip, pid, local);
}

LocationID *CommandEvent::getDestination() { 
    int ip = _psp->get<int>(DEST_IPID); 
    int pid = _psp->get<int>(DEST_PROCESSID); 
    int local = _psp->get<int>(DEST_LOCALID);
    return new LocationID(ip, pid, local);
}

/** \brief destructor
  */
CommandEvent::~CommandEvent() {
}

}
}
}
