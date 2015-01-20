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

/** \file EventSystem.h
  *
  * \ingroup events
  *
  * \brief defines the EventSystem class
  *
  */

#ifndef LSST_CTRL_EVENTS_EVENTSYSTEM_H
#define LSST_CTRL_EVENTS_EVENTSYSTEM_H

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>

#include "lsst/pex/logging/Component.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/ctrl/events.h"
#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/StatusEvent.h"
#include "lsst/ctrl/events/CommandEvent.h"
#include "lsst/ctrl/events/EventBroker.h"

using lsst::daf::base::PropertySet;

using namespace std;

namespace lsst {
namespace ctrl {
namespace events {

/**
 * @brief Coordinate publishing and receiving events
 */
class EventSystem {
public:
    EventSystem();

    ~EventSystem();

    static EventSystem& getDefaultEventSystem();

    void createTransmitter(const std::string& hostName, const std::string& topicName, int hostPort = EventBroker::DEFAULTHOSTPORT);


    void createReceiver(const std::string& hostName, const std::string& topicName, int hostPort = EventBroker::DEFAULTHOSTPORT);

    void createReceiver(const std::string& hostName, const std::string& topicName, const std::string& selector, int hostPort = EventBroker::DEFAULTHOSTPORT);

    void publishEvent(const std::string& topicName, Event& event);

    Event* receiveEvent(const std::string& topicName);
    Event* receiveEvent(const std::string& topicName, const long timeout);

    LocationID *createOriginatorID(); // deprecated

    StatusEvent* castToStatusEvent(Event* event);
    CommandEvent* castToCommandEvent(Event* event);

private:
    boost::shared_ptr<EventTransmitter> getTransmitter(const std::string& name);
    boost::shared_ptr<EventReceiver> getReceiver(const std::string& name);

protected:
    static EventSystem *defaultEventSystem;

    static short _localId;
    static int _IPId;

    list<boost::shared_ptr<EventTransmitter> >_transmitters;
    list<boost::shared_ptr<EventReceiver> >_receivers;
};
}
}
}

#endif /*end LSST_CTRL_EVENTS_EVENTSYSTEM_H*/

