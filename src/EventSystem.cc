// -*- lsst-c++ -*-
/** \file EventSystem.cc
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
#include "lsst/ctrl/events/EventLog.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/ctrl/events/EventLibrary.h"

namespace pexLogging =lsst::pex::logging;
namespace pexExceptions =lsst::pex::exceptions;

using namespace std;

namespace lsst {
namespace ctrl {
namespace events {
/** \brief EventSystem object.  This object allows creation of the
  *        system's event transmitters and receivers, which can be
  *        specified at the beginning, and later used by specifying
  *        the topic to receive from or send on.
  */
EventSystem::EventSystem() {
    EventLibrary().initializeLibrary();
}

/** \brief destructor
  */
EventSystem::~EventSystem() {
}

/** \brief return the default EventSystem object, which can access all 
  *               previously created Transmitters and receivers
  * \return The EventSystem object
  */
EventSystem& EventSystem::getDefaultEventSystem() {
    if (defaultEventSystem == 0) defaultEventSystem = new EventSystem();
    return *defaultEventSystem;
}
EventSystem *EventSystem::defaultEventSystem = 0;

/** \brief create an EventTransmitter to send messages to the message broker
  * \param hostName the location of the message broker to use
  * \param topicName the topic to transmit events to
  */ 
void EventSystem::createTransmitter(const std::string& hostName, const std::string& topicName) {
    boost::shared_ptr<EventTransmitter> transmitter(new EventTransmitter(hostName, EventSystem::DEFAULTHOSTPORT, topicName));
    _transmitters.push_back(transmitter);
}

/** \brief create an EventTransmitter to send messages to the message broker
  * \param hostName the location of the message broker to use
  * \param hostPort the port where the broker can be reached
  * \param topicName the topic to transmit events to
  */ 
void EventSystem::createTransmitter(const std::string& hostName, const int hostPort, const std::string& topicName) {
    boost::shared_ptr<EventTransmitter> transmitter(new EventTransmitter(hostName, hostPort, topicName));
    _transmitters.push_back(transmitter);
}

/** \brief create an EventTransmitter to send messages to the message broker
  * \param policy the Policy object to use to configure the EventTransmitter
  */
void EventSystem::createTransmitter(const pexPolicy::Policy& policy) {
    boost::shared_ptr<EventTransmitter> transmitter(new EventTransmitter(policy));
    _transmitters.push_back(transmitter);
}

/** \brief create an EventReceiver which will receive message
  * \param hostName the location of the message broker to use
  * \param topicName the topic to receive messages from
  */
void EventSystem::createReceiver(const std::string& hostName, const std::string& topicName) {
    boost::shared_ptr<EventReceiver> receiver(new EventReceiver(hostName, EventSystem::DEFAULTHOSTPORT, topicName));
    _receivers.push_back(receiver);
}

/** \brief create an EventReceiver which will receive message
  * \param hostName the location of the message broker to use
  * \param hostPort the port where the broker can be reached
  * \param topicName the topic to receive messages from
  */
void EventSystem::createReceiver(const std::string& hostName, const int hostPort, const std::string& topicName) {
    boost::shared_ptr<EventReceiver> receiver(new EventReceiver(hostName, hostPort, topicName));
    _receivers.push_back(receiver);
}

/** \brief create an EventReceiver to receive messages from the message broker
  * \param policy the Policy object to use to configure the EventReceiver
  */
void EventSystem::createReceiver(const pexPolicy::Policy& policy) {
    boost::shared_ptr<EventReceiver> receiver(new EventReceiver(policy));
    _receivers.push_back(receiver);
}

/** \brief send an event on a topic
  * \param topicName the topic to send messages to
  * \param psp the PropertySet to send
  * \throw Runtime exception if the topic wasn't already registered using 
  *        the createTransmitter method
  */
void EventSystem::publish(const std::string& topicName, const PropertySet::Ptr psp) {
    boost::shared_ptr<EventTransmitter> transmitter;
    if ((transmitter = getTransmitter(topicName)) == 0) {
    throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "topic "+ topicName + " is not registered with EventSystem");
    }
    transmitter->publish( psp);
}

/** \brief send an logging event
  * \param topicName the topic to send messages to
  * \param rec the LogRecord to send
  * \throw Runtime exception if the topic wasn't already registered using 
  *        the createTransmitter method
  */
void EventSystem::publish(const std::string& topicName, const pexLogging::LogRecord& rec) {
    boost::shared_ptr<EventTransmitter> transmitter;
    if ((transmitter = getTransmitter(topicName)) == 0) {
    throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "topic "+ topicName + " is not registered with EventSystem");
    }
    transmitter->publish(rec);
}


/** private method to retrieve a transmitter from the internal list
  */
boost::shared_ptr<EventTransmitter> EventSystem::getTransmitter(const std::string& name) {
    list<boost::shared_ptr<EventTransmitter> >::iterator i;
    for (i = _transmitters.begin(); i != _transmitters.end(); i++) {
        if ((*i)->getTopicName() == name)
            return *i;
    }
    return boost::shared_ptr<EventTransmitter>();
}


/** \brief blocking receive for events.  Waits until an event
  *        is received for the topic specified in the constructor
  * \param topicName the topic to listen on
  * \return a PropertySet::Ptr object
  */
PropertySet::Ptr EventSystem::receive(const std::string& topicName) {
    return receive(topicName, EventReceiver::infiniteTimeout);
}

/** \brief blocking receive for events, with timeout (in milliseconds).  
  *        Waits until an event is received for the topic specified
  *        in the constructor, or until the timeout expires.      
  * \param topicName the topic to listen on
  * \param timeout the time in milliseconds to wait before returning
  * \return a Property::Ptr object on success, 0 on failure  see note
  *         in receive()
  */
PropertySet::Ptr EventSystem::receive(const std::string& topicName, const long timeout) {
    boost::shared_ptr<EventReceiver> receiver;
    if ((receiver = getReceiver(topicName)) == 0) {
    throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "Topic "+ topicName +" is not registered with EventSystem");
    }
    return receiver->receive(timeout);
}

/** private method used to retrieve the named EventReceiver object
  */
boost::shared_ptr<EventReceiver> EventSystem::getReceiver(const std::string& name) {
    list<boost::shared_ptr<EventReceiver> >::iterator i;
    for (i = _receivers.begin(); i != _receivers.end(); i++) {
        if ((*i)->getTopicName() == name)
            return *i;
    }
    return boost::shared_ptr<EventReceiver>();
}

}
}
}
