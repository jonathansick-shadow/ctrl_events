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
 * @file EventSystem.h
 *
 * @ingroup ctrl/events
 *
 * @brief defines the EventSystem class
 *
 */

#ifndef LSST_CTRL_EVENTS_EVENTSYSTEM_H
#define LSST_CTRL_EVENTS_EVENTSYSTEM_H

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>

#include "lsst/daf/base/PropertySet.h"

#include "lsst/ctrl/events/EventTransmitter.h"
#include "lsst/ctrl/events/EventEnqueuer.h"
#include "lsst/ctrl/events/EventReceiver.h"
#include "lsst/ctrl/events/EventDequeuer.h"
#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/StatusEvent.h"
#include "lsst/ctrl/events/CommandEvent.h"
#include "lsst/ctrl/events/EventBroker.h"

using lsst::daf::base::PropertySet;

namespace lsst {
namespace ctrl {
namespace events {

/**
 * @class EventSystem
 * @brief This object allows creation of the system's event transmitters
 *        and receivers, which can be specified at the beginning, and later
 *        used by specifying the destination to receive from or send on.
 */
class EventSystem {
public:
    /**
     * @brief destructor
     */
    ~EventSystem();

    /**
     * @brief return the default EventSystem object, which can access all
     *               previously created Transmitters and receivers
     * @return The EventSystem object
     */
    static EventSystem& getDefaultEventSystem();

    /**
     * @brief create an EventTransmitter to send messages to the message broker
     * @param hostName the location of the message broker to use
     * @param topicName the topic to transmit events to
     * @param hostPort the port where the broker can be reached
     * @throws lsst::pex::exceptions::RuntimeError if topic is already registered
     */
    void createTransmitter(std::string const& hostName, std::string const& topicName, int hostPort = EventBroker::DEFAULTHOSTPORT);

    /**
     * @brief create a EventQueuer to send messages to the message broker
     * @param hostName the location of the message broker to use
     * @param queueName the queue to transmit events to
     * @param hostPort the port where the broker can be reached
     * @throws lsst::pex::exceptions::RuntimeError if queue is already registered
     */
    void createEnqueuer(std::string const& hostName, std::string const& queueName, int hostPort = EventBroker::DEFAULTHOSTPORT);

    /**
     * @brief create an EventReceiver which will receive message
     * @param hostName the location of the message broker to use
     * @param topicName the topic to receive messages from
     * @param hostPort the port where the broker can be reached
     * @throws lsst::pex::exceptions::RuntimeError if topic is already registered
     */
    void createReceiver(std::string const& hostName, std::string const& topicName, int hostPort = EventBroker::DEFAULTHOSTPORT);

    /**
     * @brief create a EventDequeuer which will receive message
     * @param hostName the location of the message broker to use
     * @param queueName the queue to receive messages from
     * @param hostPort the port where the broker can be reached
     * @throws lsst::pex::exceptions::RuntimeError if topic is already registered
     */
    void createDequeuer(std::string const& hostName, std::string const& queueName, int hostPort = EventBroker::DEFAULTHOSTPORT);

    /**
     * @brief create an EventReceiver which will receive message
     * @param hostName the location of the message broker to use
     * @param topicName the topic to receive messages from
     * @param selector the message selector to specify which messages to receive
     * @param hostPort the port where the broker can be reached
     * @note The selector allows filtering of messages on the broker before the event is received
     */
    void createReceiver(std::string const& hostName, std::string const& topicName, std::string const& selector, int hostPort = EventBroker::DEFAULTHOSTPORT);

    /**
     * @brief create a EventDequeuer which will receive message
     * @param hostName the location of the message broker to use
     * @param queueName the queue to receive messages from
     * @param selector the message selector to specify which messages to receive
     * @param hostPort the port where the broker can be reached
     * @note The selector allows filtering of messages on the broker before the event is received
     */
    void createDequeuer(std::string const& hostName, std::string const& queueName, std::string const& selector, int hostPort = EventBroker::DEFAULTHOSTPORT);

    /**
     * @brief send an event to a destination
     * @param destinationName the destination to send messages to
     * @param event the Event to send
     * @throws Runtime exception if the destination wasn't already registered using
     *        the createTransmitter method
     */
    void publishEvent(std::string const& destinationName, Event& event);

    /**
     * @brief blocking receive for events.  Waits until an event
     *        is received for the destination specified in the constructor
     * @param destinationName the destination to listen on
     * @return an Event object
     */
   PTR(Event) receiveEvent(std::string const& destinationName);

    /**
     * @brief blocking receive for events, with timeout (in milliseconds).
     *        Waits until an event is received for the destination specified
     *        in the constructor, or until the timeout expires.
     * @param destinationName the destination to listen on
     * @param timeout the time in milliseconds to wait before returning
     * @return an Event object on success, 0 on failure
     */
    PTR(Event) receiveEvent(std::string const& destinationName, const long timeout);

    /**
     * @brief create an LocationId
     * @return a LocationId 
     */
    PTR(LocationId) createOriginatorId() const;

    /**
     * @brief cast an Event to StatusEvent
     * @param event an Event
     * @return a StatusEvent
     */
    PTR(StatusEvent) castToStatusEvent(PTR(Event) event);

    /**
     * @brief cast an Event to CommandEvent
     * @param event an Event
     * @return a CommandEvent
     */
    PTR(CommandEvent) castToCommandEvent(PTR(Event) event);

private:
    static EventSystem *defaultEventSystem;

    PTR(Transmitter) getTransmitter(std::string const& name);
    PTR(Receiver) getReceiver(std::string const& name);

protected:
    EventSystem();


    static std::list<PTR(EventTransmitter) >_transmitters;
    static std::list<PTR(EventReceiver) >_receivers;

    static std::list<PTR(EventEnqueuer) >_enqueuers;
    static std::list<PTR(EventDequeuer) >_dequeuers;
};

}}}

#endif /*end LSST_CTRL_EVENTS_EVENTSYSTEM_H*/
