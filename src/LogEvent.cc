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

/** \file LogEvent.cc
  *
  * \brief Log Event implementation
  *
  * \ingroup ctrl/events
  *
  */
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <cstring>

#include "lsst/ctrl/events/EventTypes.h"
#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/LogEvent.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/exceptions.h"

#include <log4cxx/helpers/stringhelper.h>
#include <log4cxx/helpers/pool.h>
#include <log4cxx/spi/location/locationinfo.h>

using namespace log4cxx;
using namespace log4cxx::helpers;

namespace pexExceptions = lsst::pex::exceptions;


using namespace std;

namespace lsst {
namespace ctrl {
namespace events {

const std::string LogEvent::LEVEL = "LEVEL";
const std::string LogEvent::LOGGER = "LOG";
const std::string LogEvent::LOGGING_TOPIC = "DM_LOGGING";
const int LogEvent::BROKER_PORT = 61616;

const std::string LogEvent::MESSAGE = "message";
const std::string LogEvent::TIMESTAMP = "timestamp";
const std::string LogEvent::THREADNAME = "threadname";
const std::string LogEvent::FILENAME = "filename";
const std::string LogEvent::CLASSNAME = "classname";
const std::string LogEvent::METHODNAME = "methodname";
const std::string LogEvent::LINENUMBER = "linenumber";
const std::string LogEvent::LOCATION = "location";

/** \brief Creates LogEvent which contains a PropertySet
  *
  */
LogEvent::LogEvent() : Event() {
    _init();
}

LogEvent::LogEvent(const log4cxx::spi::LoggingEventPtr& event, log4cxx::helpers::Pool& p) : Event() {
    _init();

    _psp->set(TYPE, EventTypes::LOG);
    _psp->set(LOGGER, event->getLoggerName());
    _psp->set(LEVEL, event->getLevel()->toInt());

    // commented out for now... this info is sent in the message header
    // for broker-side parsing.

    _psp->set(MESSAGE, event->getMessage());
    _psp->set(TIMESTAMP, event->getTimeStamp());
    _psp->set(THREADNAME, event->getThreadName());

    spi::LocationInfo location = event->getLocationInformation();

    PropertySet::Ptr loc(new PropertySet);
    loc->set(FILENAME, location.getFileName());
    loc->set(CLASSNAME, location.getClassName());
    loc->set(METHODNAME, location.getMethodName());
    loc->set(LINENUMBER, location.getLineNumber());

    _psp->set(LOCATION, loc);
}


/** private method to add keywords used in LogEvent JMS headers
  */
void LogEvent::_init() {
    _keywords.insert(LEVEL);
    _keywords.insert(LOGGER);
}

/** \brief Constructor to take a JMS TextMessage and turn it into a LogEvent
  *  \param msg a TextMessage
  */
LogEvent::LogEvent(cms::TextMessage *msg) : Event(msg) {
    _init();

    _psp->set(LogEvent::LEVEL, msg->getIntProperty(LogEvent::LEVEL));

    _psp->set(LogEvent::LOGGER, msg->getStringProperty(LogEvent::LOGGER));

}

/** private method used to populate the LogEvent
  */

void LogEvent::populateHeader(cms::TextMessage* msg) const {
    Event::populateHeader(msg);

    msg->setIntProperty(LogEvent::LEVEL, _psp->get<int>(LogEvent::LEVEL));

    msg->setStringProperty(LogEvent::LOGGER, _psp->get<std::string>(LogEvent::LOGGER));

}

/** \brief retreive the log level
  * \return the logging level at which the LogRecord message was set
  */
int LogEvent::getLevel() {
    return _psp->get<int>(LogEvent::LEVEL);
}

/** \brief Retreive the log message 
  * \return a string containing the log message itself
  */
std::string LogEvent::getLogger() {
    return _psp->get<std::string>(LogEvent::LOGGER);
}

/** \brief destructor
  */
LogEvent::~LogEvent() {
}

}
}
}
