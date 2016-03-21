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
 * @file EventFactory.cc
 *
 * @ingroup ctrl/events
 *
 * @brief Create the proper type of event, given a cms::TextMessage
 *
 */

#include "lsst/ctrl/events/StatusEvent.h"
#include "lsst/ctrl/events/CommandEvent.h"
#include "lsst/ctrl/events/LogEvent.h"
#include "lsst/ctrl/events/EventTypes.h"

#include "lsst/ctrl/events/EventFactory.h"

namespace lsst {
namespace ctrl {
namespace events {

EventFactory::EventFactory() {
}

EventFactory::~EventFactory() {
}

PTR(Event) EventFactory::createEvent(cms::TextMessage* msg) {
    std::vector<std::string> names = msg->getPropertyNames();

    std::string _type = msg->getStringProperty("TYPE");

    if (_type == EventTypes::LOG) {
        return PTR(LogEvent)(new LogEvent(msg));
    } else if (_type == EventTypes::STATUS) {
        return PTR(StatusEvent)(new StatusEvent(msg));
    } else if (_type == EventTypes::COMMAND) {
        return PTR(CommandEvent)(new CommandEvent(msg));
    } else {
        return PTR(Event)(new Event(msg));
    }
}

}}}
