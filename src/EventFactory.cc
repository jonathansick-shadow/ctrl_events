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

/** \file EventFactory.cc
  *
  * \brief Coordinate EventTransmitters and EventReceiver objects
  *
  * \ingroup events
  *
  * \author Stephen R. Pietrowicz, NCSA
  *
  */
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/logging/LogRecord.h"
#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/exceptions.h"

#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/StatusEvent.h"
#include "lsst/ctrl/events/CommandEvent.h"
#include "lsst/ctrl/events/LogEvent.h"
#include "lsst/ctrl/events/EventTypes.h"

#include "lsst/ctrl/events/EventLog.h"
#include "lsst/ctrl/events/EventFactory.h"
#include "lsst/ctrl/events/EventLibrary.h"

namespace pexLogging =lsst::pex::logging;
namespace pexExceptions =lsst::pex::exceptions;

using namespace std;

namespace lsst {
namespace ctrl {
namespace events {
/** \brief EventFactory object.  This object creates Event objects
  *        from TextMessages.
  */
EventFactory::EventFactory() {
}

/** \brief destructor
  */
EventFactory::~EventFactory() {
}

/** \brief return an Event object, based on the type received in the TextMessage.
  * \return An Event object
  */
Event* EventFactory::createEvent(cms::TextMessage* msg) {
    vector<std::string> names = msg->getPropertyNames();

    std::string _type = msg->getStringProperty("TYPE");

    if (_type == EventTypes::LOG) {
        return new LogEvent(msg);
    } else if (_type == EventTypes::STATUS) {
        return new StatusEvent(msg);
    } else if (_type == EventTypes::COMMAND) {
        return new CommandEvent(msg);
    }
    return new Event(msg);
}
}

}
}
