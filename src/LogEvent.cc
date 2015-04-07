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
 * @file LogEvent.cc
 *
 * @ingroup ctrl/events
 *
 * @brief Log Event implementation
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
const std::string LogEvent::LOGGER = "LOGGER";
const std::string LogEvent::MESSAGE = "MESSAGE";
const std::string LogEvent::TIMESTAMP = "TIMESTAMP";
const std::string LogEvent::THREADNAME = "THREADNAME";
const std::string LogEvent::FILENAME = "FILENAME";
const std::string LogEvent::CLASSNAME = "CLASSNAME";
const std::string LogEvent::METHODNAME = "METHODNAME";
const std::string LogEvent::LINENUMBER = "LINENUMBER";
const std::string LogEvent::LOCATION = "LOCATION";

const std::string LogEvent::LOGGING_TOPIC = "Logging";

/** 
 * @brief Creates LogEvent which contains a PropertySet
 */

LogEvent::LogEvent() : Event() {
    _init();
}

LogEvent::LogEvent(const log4cxx::spi::LoggingEventPtr& event, log4cxx::helpers::Pool& p) : Event() {
    _init();

    _psp->set(TYPE, EventTypes::LOG);
    _psp->set(LogEvent::LOGGER, event->getLoggerName());
    _psp->set(LogEvent::LEVEL, event->getLevel()->toInt());

    _psp->set(LogEvent::MESSAGE, event->getMessage());
    _psp->set(LogEvent::TIMESTAMP, event->getTimeStamp());
    _psp->set(LogEvent::THREADNAME, event->getThreadName());

    spi::LocationInfo location = event->getLocationInformation();

    PropertySet::Ptr loc(new PropertySet);
    loc->set(LogEvent::FILENAME, location.getFileName());
    loc->set(LogEvent::CLASSNAME, location.getClassName());
    loc->set(LogEvent::METHODNAME, location.getMethodName());
    loc->set(LogEvent::LINENUMBER, location.getLineNumber());

    _psp->set(LogEvent::LOCATION, loc);
}


/** private method to add keywords used in LogEvent JMS headers
  */
void LogEvent::_init() {
    _keywords.insert(LogEvent::LEVEL);
    _keywords.insert(LogEvent::LOGGER);
}


/** 
 * @brief Constructor to take a JMS TextMessage and turn it into a LogEvent
 * @param msg a TextMessage
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

/** 
 * @brief retreive the log level
 * @return the logging level at which the LogRecord message was set
 */
int LogEvent::getLevel() {
    return _psp->get<int>(LogEvent::LEVEL);
}

/** 
 * @brief Retreive the log message 
 * @return a string containing the log message itself
 */
std::string LogEvent::getLogger() {
    return _psp->get<std::string>(LogEvent::LOGGER);
}

/** 
 * @brief Retreive the log message 
 * @return a string containing the log message itself
 */
std::string LogEvent::getLoggingTopic() {
    return LogEvent::LOGGING_TOPIC;
}

/** 
  * @brief destructor
  */
LogEvent::~LogEvent() {
}

}
}
}
