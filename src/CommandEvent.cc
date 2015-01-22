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

/** \file CommandEvent.cc
  *
  * \brief Command Event implementation
  *
  * \ingroup ctrl/events
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

const std::string CommandEvent::ORIG_HOSTNAME = "ORIG_HOSTNAME";
const std::string CommandEvent::ORIG_PROCESSID = "ORIG_PROCESSID";
const std::string CommandEvent::ORIG_LOCALID = "ORIG_LOCALID";

const std::string CommandEvent::DEST_HOSTNAME = "DEST_HOSTNAME";
const std::string CommandEvent::DEST_PROCESSID = "DEST_PROCESSID";
const std::string CommandEvent::DEST_LOCALID = "DEST_LOCALID";

/** \brief Creates CommandEvent which contains a PropertySet
  *        consisting of an origination location ID and 
  *        a destination location ID, plus additional
  *        properties.
  */

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

/**
  * \brief Constructor for CommandEvent
  * \param msg a cms::TextMessage to convert into a CommandEvent
  */
CommandEvent::CommandEvent(cms::TextMessage *msg) : Event(msg) {
    _init();


    _psp->set(ORIG_HOSTNAME, (std::string)msg->getStringProperty(ORIG_HOSTNAME));
    _psp->set(ORIG_PROCESSID, (int)msg->getIntProperty(ORIG_PROCESSID));
    _psp->set(ORIG_LOCALID, (int)msg->getIntProperty(ORIG_LOCALID));

    _psp->set(DEST_HOSTNAME, (std::string)msg->getStringProperty(DEST_HOSTNAME));
    _psp->set(DEST_PROCESSID, (int)msg->getIntProperty(DEST_PROCESSID));
    _psp->set(DEST_LOCALID, (int)msg->getIntProperty(DEST_LOCALID));

}
/**
  * \brief Constructor for CommandEvent
  * \param originator originating location of this event
  * \param destination destination location for this event
  * \param psp PropertySet to pass in this event
  */
CommandEvent::CommandEvent(const LocationID&  originator, const LocationID& destination, const PropertySet::Ptr psp) : Event(*psp) {
    _constructor(originator, destination);
}

/**
  * \brief Constructor for CommandEvent
  * \param originator originating location of this event
  * \param destination destination location for this event
  * \param ps PropertySet to pass in this event
  */
CommandEvent::CommandEvent(const LocationID&  originator, const LocationID&  destination, const PropertySet& ps) : Event(ps) {
    _constructor(originator, destination);
}

/**
  * \brief Constructor for CommandEvent
  * \param originator originating location of this event
  * \param destination destination location for this event
  * \param ps PropertySet to pass in this event
  * \param filterable additional, broker-filterable, PropertySet parameters
  */
CommandEvent::CommandEvent(const LocationID&  originator, const LocationID&  destination, const PropertySet& ps, const PropertySet& filterable) : Event(ps, filterable) {
    _constructor(originator, destination);
}

/**
  * \brief Constructor for CommandEvent
  * \param runId name of the run which this event is used in
  * \param originator originating location of this event
  * \param destination destination location for this event
  * \param psp PropertySet to pass in this event
  */
CommandEvent::CommandEvent( const std::string& runId, const LocationID&  originator, const LocationID& destination, const PropertySet::Ptr psp) : Event(runId, *psp) {
    _constructor(originator, destination);
}

/**
  * \brief Constructor for CommandEvent
  * \param runId name of the run which this event is used in
  * \param originator originating location of this event
  * \param destination destination location for this event
  * \param ps PropertySet to pass in this event
  */
CommandEvent::CommandEvent( const std::string& runId, const LocationID&  originator, const LocationID&  destination, const PropertySet& ps) : Event(runId, ps) {
    _constructor(originator, destination);
}

/**
  * \brief Constructor for CommandEvent
  * \param runId name of the run which this event is used in
  * \param originator originating location of this event
  * \param destination destination location for this event
  * \param ps PropertySet to pass in this event
  * \param filterable additional, broker-filterable, PropertySet parameters
  */
CommandEvent::CommandEvent( const std::string& runId, const LocationID&  originator, const LocationID&  destination, const PropertySet& ps, const PropertySet& filterable) : Event(runId, ps, filterable) {
    _constructor(originator, destination);
}

/** private method common to all constructors containing, originator, the 
  * originating location of this event, and destination, the destination
  * location for this event.
  */
void CommandEvent::_constructor(const LocationID&  originator, const LocationID&  destination) {
    _init();

    _psp->set(ORIG_HOSTNAME, originator.getHostName());
    _psp->set(ORIG_PROCESSID, originator.getProcessID());
    _psp->set(ORIG_LOCALID, originator.getLocalID());

    _psp->set(DEST_HOSTNAME, destination.getHostName());
    _psp->set(DEST_PROCESSID, destination.getProcessID());
    _psp->set(DEST_LOCALID, destination.getLocalID());

    _psp->set(TYPE, EventTypes::COMMAND);

}

/** private method to initialize CommandEvent internal info into the given
  * TextMessage object
  */
void CommandEvent::populateHeader(cms::TextMessage* msg) const {
    Event::populateHeader(msg);

    msg->setStringProperty(ORIG_HOSTNAME, _psp->get<std::string>(ORIG_HOSTNAME));
    msg->setIntProperty(ORIG_PROCESSID, _psp->get<int>(ORIG_PROCESSID));
    msg->setIntProperty(ORIG_LOCALID, _psp->get<int>(ORIG_LOCALID));

    msg->setStringProperty(DEST_HOSTNAME, _psp->get<std::string>(DEST_HOSTNAME));
    msg->setIntProperty(DEST_PROCESSID, _psp->get<int>(DEST_PROCESSID));
    msg->setIntProperty(DEST_LOCALID, _psp->get<int>(DEST_LOCALID));
}

/**
  * \brief retrieve an object containing the OriginatoDesination LocationID
  */
LocationID *CommandEvent::getOriginator() { 
    std::string hostname =  _psp->get<std::string>(ORIG_HOSTNAME);
    int pid =  _psp->get<int>(ORIG_PROCESSID);
    int local =  _psp->get<int>(ORIG_LOCALID);
    return new LocationID(hostname, pid, local);
}

/**
  * \brief retrieve an object containing the Desination LocationID
  */
LocationID *CommandEvent::getDestination() { 
    std::string hostname = _psp->get<std::string>(DEST_HOSTNAME); 
    int pid = _psp->get<int>(DEST_PROCESSID); 
    int local = _psp->get<int>(DEST_LOCALID);
    return new LocationID(hostname, pid, local);
}

/** \brief destructor
  */
CommandEvent::~CommandEvent() {
}

}
}
}
