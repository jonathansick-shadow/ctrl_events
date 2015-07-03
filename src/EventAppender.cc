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

/** \file EventAppender.cc
  *
  * \brief send log4cxx log messages out as events
  *
  */
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <cstring>
#include <unistd.h>

#include <log4cxx/helpers/stringhelper.h>
#include <log4cxx/helpers/pool.h>
#include <log4cxx/spi/location/locationinfo.h>
#include <log4cxx/helpers/loglog.h>

#include "lsst/ctrl/events/EventBroker.h"
#include "lsst/ctrl/events/EventAppender.h"
#include "lsst/ctrl/events/EventSystem.h"

#include "lsst/pex/exceptions.h"

namespace pexExceptions = lsst::pex::exceptions;

using namespace log4cxx;
using namespace log4cxx::helpers;

namespace ctrlEvents = lsst::ctrl::events;

namespace lsst {
namespace ctrl {
namespace events {

LSST_IMPLEMENT_LOG4CXX_OBJECT(EventAppender)

EventAppender::EventAppender() {
    _transmitter = NULL;
    _port = ctrlEvents::EventBroker::DEFAULTHOSTPORT;
    _topic = ctrlEvents::LogEvent::LOGGING_TOPIC;
}

/** \brief destructor
  */
EventAppender::~EventAppender() {
    if (_transmitter != NULL)
        delete _transmitter;
    _transmitter = NULL;
}

void EventAppender::setOption(const LogString& option, const LogString& value) {
    if (StringHelper::equalsIgnoreCase(option, LOG4CXX_STR("BROKER"), LOG4CXX_STR("broker"))) {
        _broker = value;
    } else if (StringHelper::equalsIgnoreCase(option, LOG4CXX_STR("PORT"), LOG4CXX_STR("port"))) {
        _port = atoi(value.c_str());
    } else if (StringHelper::equalsIgnoreCase(option, LOG4CXX_STR("TOPIC"), LOG4CXX_STR("topic"))) {
        _topic = value;
    } else {
        AppenderSkeleton::setOption(option, value);
    }
}

void EventAppender::activateOptions(Pool&)
{
    if (_broker.empty()) {
        LogLog::error((LogString) LOG4CXX_STR("Appender requires broker option to be specified"));
        return;
    }
    try {
        _transmitter = new EventTransmitter(_broker, _topic, _port);
    } catch (pexExceptions::RuntimeError& rte) {
        std::ostringstream msg;
        msg << "Couldn't reach broker " << _broker << " at port " << _port;
        LogLog::error((LogString) LOG4CXX_STR(msg.str()));
        _transmitter = NULL;
    }
}


EventTransmitter* EventAppender::getTransmitter() {
    if (_transmitter == NULL) {
        try {
            _transmitter = new EventTransmitter(_broker, _topic, _port);
        } catch (Exception& e) {
        }
    }
    return _transmitter;
}
    

void EventAppender::append(const spi::LoggingEventPtr& event, helpers::Pool& p) {
    PropertySet::Ptr psp(new PropertySet);

    psp->set("logger", event->getLoggerName());
    psp->set("message", event->getMessage());
    psp->set("timestamp", event->getTimeStamp());
    psp->set("threadname", event->getThreadName());
    psp->set("level", event->getLevel()->toInt());

    spi::LocationInfo location = event->getLocationInformation();

    PropertySet::Ptr loc(new PropertySet);
    loc->set("filename", location.getFileName());
    loc->set("classname", location.getClassName());
    loc->set("methodname", location.getMethodName());
    loc->set("linenumber", location.getLineNumber());

    psp->set("location", loc);

    ctrlEvents::Event e = ctrlEvents::Event(*psp);

    EventTransmitter *transmitter = getTransmitter();

    if (_transmitter == NULL)
        return;

    try {
        transmitter->publishEvent(e);
    } catch (Exception& e) {
        LogLog::error((LogString) LOG4CXX_STR("event transmitter failed"));
        delete _transmitter;
        _transmitter = NULL;
    }
}

void EventAppender::close() {
    if (this->closed)
        return;
    this->closed = true;
}

}}}
