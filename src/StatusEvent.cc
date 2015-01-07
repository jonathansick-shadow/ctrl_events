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
 
/** \file StatusEvent.cc
  *
  * \brief Status Event implementation
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

#include "lsst/ctrl/events/OriginatorID.h"
#include "lsst/ctrl/events/EventTypes.h"
#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/StatusEvent.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/daf/base/DateTime.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/exceptions.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <time.h>

#include <activemq/core/ActiveMQConnectionFactory.h>

namespace pexExceptions = lsst::pex::exceptions;
namespace pexLogging = lsst::pex::logging;


using namespace std;
using std::numeric_limits;

namespace lsst {
namespace ctrl {
namespace events {

const std::string StatusEvent::ORIG_LOCALID = "ORIG_LOCALID";

/** \brief Creates StatusEvent which contains a PropertySet
  *
  */

StatusEvent::StatusEvent() : Event() {
    _init();
}


void StatusEvent::_init() {
    _keywords.insert(ORIG_LOCALID);
}

StatusEvent::StatusEvent(cms::TextMessage *msg) : Event(msg) {
    _init();

    _psp->set(ORIG_LOCALID, (int)msg->getIntProperty(ORIG_LOCALID));
}

StatusEvent::StatusEvent( const std::string& runID, const OriginatorID& originatorID, const PropertySet::Ptr psp) : Event(runID, *psp) {
    _constructor(runID, originatorID, *psp);
}

StatusEvent::StatusEvent( const std::string& runID, const OriginatorID& originatorID, const PropertySet& ps) : Event(runID, ps) {
    _constructor(runID, originatorID, ps);
}

void StatusEvent::_constructor(const std::string& runID, const OriginatorID& originatorID, const PropertySet& ps) {
    _init();


    _psp->set(HOSTIP, originatorID.getIPAddress());
    _psp->set(HOSTPROCESS, originatorID.getProcessID());
    _psp->set(ORIG_LOCALID, originatorID.getLocalID());
    _psp->set(TYPE, EventTypes::STATUS);

}

void StatusEvent::populateHeader(cms::TextMessage* msg) const {
    Event::populateHeader(msg);

    msg->setIntProperty(HOSTIP, _psp->get<int>(HOSTIP));
    msg->setIntProperty(HOSTPROCESS, _psp->get<int>(HOSTPROCESS));
    msg->setIntProperty(ORIG_LOCALID, _psp->get<int>(ORIG_LOCALID));
}

OriginatorID *StatusEvent::getOriginatorId() {
    int ip = _psp->get<int>(HOSTIP);
    int pid = _psp->get<int>(HOSTPROCESS);
    int local = _psp->get<int>(ORIG_LOCALID);
    return new OriginatorID(ip, pid, local);
}

/** \brief destructor
  */
StatusEvent::~StatusEvent() {
}

}
}
}
