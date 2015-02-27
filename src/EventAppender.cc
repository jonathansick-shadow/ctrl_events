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
#include <syscall.h>

#include "lsst/ctrl/events/EventAppender.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/ctrl/events/EventBroker.h"
#include <log4cxx/helpers/stringhelper.h>
#include <log4cxx/helpers/pool.h>
#include <log4cxx/spi/location/locationinfo.h>

using namespace log4cxx;
using namespace log4cxx::helpers;
namespace ctrlEvents = lsst::ctrl::events;

IMPLEMENT_LOG4CXX_OBJECT(EventAppender)


EventAppender::EventAppender() {
    std::cout << "event appender object created" << std::endl;
    _transmitterInitialized = 0;
    broker_port = ctrlEvents::LogEvent::BROKER_PORT;
    logging_topic = ctrlEvents::LogEvent::LOGGING_TOPIC;
}


void EventAppender::setOption(const LogString& option, const LogString& value) {
    std::cout << "attempting to set options!" << std::endl;
    if (StringHelper::equalsIgnoreCase(option, LOG4CXX_STR("BROKER_HOST"), LOG4CXX_STR("broker_host"))) {
        broker_host = value;
        std::cout << "broker set! [" << value << "]" << std::endl;
    } else if (StringHelper::equalsIgnoreCase(option, LOG4CXX_STR("BROKER_PORT"), LOG4CXX_STR("broker_port"))) {
        std::cout << "setting port..." << std::endl;
        broker_port = atoi(value.c_str());
        std::cout << "port set: " << broker_port << std::endl;
    } else if (StringHelper::equalsIgnoreCase(option, LOG4CXX_STR("LOGGING_TOPIC"), LOG4CXX_STR("logging_topic"))) {
        logging_topic = value;
        std::cout << "logging_topic set! [" << value << "]" << std::endl;
    } else {
        AppenderSkeleton::setOption(option, value);
    }
    std::cout << "done attempting to set options!" << std::endl;
}


void EventAppender::append(const spi::LoggingEventPtr& event, log4cxx::helpers::Pool& p) {
    ctrlEvents::LogEvent e = ctrlEvents::LogEvent(event, p);
    if (_transmitterInitialized == 0)  {
        _transmitter = new ctrlEvents::EventTransmitter(broker_host, logging_topic, broker_port);
        std::cout << "created!" << std::endl;
        _transmitterInitialized = 1;
    }
    std::cout << "publishing!" << std::endl;
    _transmitter->publishEvent(e);
}

void EventAppender::close() {
    if (this->closed)
        return;
    this->closed = true;
}

/** \brief destructor
  */
EventAppender::~EventAppender() {
    delete _transmitter;
}
