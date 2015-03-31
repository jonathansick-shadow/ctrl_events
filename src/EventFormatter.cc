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
 * @file EventFormatter.cc
 *
 * @ingroup ctrl/events
 *
 * @brief EventFormatter class required for logging support for events
 *
 */
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/ctrl/events/EventLog.h"
#include "lsst/ctrl/events/LogEvent.h"
#include "lsst/ctrl/events/EventFormatter.h"
#include "lsst/pex/logging/LogRecord.h"
#include "lsst/pex/logging/Component.h"


using namespace std;
namespace pexLogging = lsst::pex::logging;

namespace lsst {
namespace ctrl {
namespace events {

EventFormatter::~EventFormatter() {}


/** \brief writes a record to the event log stream.   This ignores the
  *        ostream, but this is required because of the signature of this
  *        required method
  * \param os output stream.  this is ignored, since the output stream
  *           is the event stream; it's a placeholder because of the
  *           signature of this method.
  * \param rec the LogRecord to send to the event topic;
  */
void EventFormatter::write(ostream *os, const pexLogging::LogRecord& rec) {
    EventSystem system = EventSystem::getDefaultEventSystem();

    const PropertySet& ps = rec.getProperties();
    
    if (!ps.exists(Event::RUNID)) {
        LogEvent event("unspecified",rec);
        system.publishEvent(EventLog::LOGGING_TOPIC, event);
        return;
    } 
    std::string runid = ps.get<std::string>(Event::RUNID);
    LogEvent event(runid,rec);
    system.publishEvent(EventLog::LOGGING_TOPIC, event);
}

}
}
}
