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
}

/** \brief destructor
  */
EventSystem::~EventSystem() {
}

void EventSystem::destroyTransmitter(const std::string& topicName) {
    list<boost::shared_ptr<EventTransmitter> >::iterator i;
    list<boost::shared_ptr<EventTransmitter> >::iterator i2;
    for (i = _transmitters.begin(); i != _transmitters.end(); i++) {
        if ((*i)->getTopicName() == topicName) {
            i2 = _transmitters.erase(i);
            return;
        } 
    }
    throw LSST_EXCEPT(pexExceptions::NotFoundException, "could not find transmitter with specified topicName");
}

void EventSystem::destroyReceiver(const std::string& topicName) {
    list<boost::shared_ptr<EventReceiver> >::iterator i;
    list<boost::shared_ptr<EventReceiver> >::iterator i2;
    for (i = _receivers.begin(); i != _receivers.end(); i++) {
        if ((*i)->getTopicName() == topicName) {
            i2 = _receivers.erase(i);
            return;
        } 
    }
    throw LSST_EXCEPT(pexExceptions::NotFoundException, "could not find receiver with specified topicName");
}

void EventSystem::destroyTransmitter(const pexPolicy::Policy& policy) {
    std::string topicName;

    try {
        topicName = policy.getString("topicName");
    } catch (pexPolicy::NameNotFound& e) {
        throw LSST_EXCEPT(pexExceptions::NotFoundException, "topicName was not specified in policy");
    }

    destroyTransmitter(topicName);
}
    
void EventSystem::destroyReceiver(const pexPolicy::Policy& policy) {
    std::string topicName;

    try {
        topicName = policy.getString("topicName");
    } catch (pexPolicy::NameNotFound& e) {
        throw LSST_EXCEPT(pexExceptions::NotFoundException, "topicName was not specified in policy");
    }

    destroyReceiver(topicName);
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
    boost::shared_ptr<EventTransmitter> transmitter(new EventTransmitter(hostName, topicName));
    _transmitters.push_back(transmitter);
}

/** \brief create an EventTransmitter to send messages to the message broker
  * \param policy the Policy object to use to configure the EventTransmitter
  */
void EventSystem::createTransmitter(const pexPolicy::Policy& policy) {
    boost::shared_ptr<EventTransmitter> transmitter(new EventTransmitter(policy));
    _transmitters.push_back(transmitter);
}

void EventSystem::createLocalTransmitter(const std::string& topicName) {
    pexPolicy::Policy policy;
    policy.set("useLocalSockets", true);
    policy.set("topicName", topicName);
    boost::shared_ptr<EventTransmitter> transmitter(new EventTransmitter(policy));
    _transmitters.push_back(transmitter);
}

/** \brief create an EventReceiver which will receive message
  * \param hostName the location of the message broker to use
  * \param topicName the topic to receive messages from
  */
void EventSystem::createReceiver(const std::string& hostName, const std::string& topicName) {
    boost::shared_ptr<EventReceiver> receiver(new EventReceiver(hostName, topicName));
    _receivers.push_back(receiver);
}

/** \brief create an EventReceiver to receive messages from the message broker
  * \param policy the Policy object to use to configure the EventReceiver
  */
void EventSystem::createReceiver(const pexPolicy::Policy& policy) {
    boost::shared_ptr<EventReceiver> receiver(new EventReceiver(policy));
    _receivers.push_back(receiver);
}

/** \brief create an EventReceiver that uses local sockets to communicate
  * \param topicName the topic to receive messages on
  */
void EventSystem::createLocalReceiver(const std::string& topicName) {
    pexPolicy::Policy policy;
    policy.set("useLocalSockets", true);
    policy.set("topicName", topicName);
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
// TODO: roll this back into the Template, once the swig stuff is working properly
PropertySet::Ptr EventSystem::matchingReceive(const std::string& topicName, const std::string& name, const int value) {
    return _matchingReceive(topicName, name, value);
}
PropertySet::Ptr EventSystem::matchingReceive(const std::string& topicName, const std::string& name, const long value) {
    return _matchingReceive(topicName, name, value);
}

PropertySet::Ptr EventSystem::matchingReceive(const std::string& topicName, const std::string& name, const float value) {
    return _matchingReceive(topicName, name, value);
}

PropertySet::Ptr EventSystem::matchingReceive(const std::string& topicName, const std::string& name, const double value) {
    return _matchingReceive(topicName, name, value);
}

PropertySet::Ptr EventSystem::matchingReceive(const std::string& topicName, const std::string& name, const long long value) {
    return _matchingReceive(topicName, name, value);
}

PropertySet::Ptr EventSystem::matchingReceive(const std::string& topicName, const std::string& name, const std::string& value) {
    return _matchingReceive(topicName, name, value);
}


/** \brief Receives events matching both the name and string value
  *             
  * \param topicName the topic to receive messages on
  * \param name the Property name to match
  * \param value the string value to match
  * \return the matching PropertySet::Ptr object
  */
template <typename T>
PropertySet::Ptr EventSystem::_matchingReceive(const std::string& topicName, const std::string& name, const T& value) {
    boost::shared_ptr<EventReceiver> receiver;
    if ((receiver = getReceiver(topicName)) == 0) {
    throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "Topic "+ topicName +" is not registered with EventSystem");
    }
    return receiver->matchingReceive(name, value);
}

PropertySet::Ptr EventSystem::matchingReceive(const std::string& topicName, const std::string& name, int value, const long timeout) {
    return _matchingReceive(topicName, name, value, timeout);
}

PropertySet::Ptr EventSystem::matchingReceive(const std::string& topicName, const std::string& name, long value, const long timeout) {
    return _matchingReceive(topicName, name, value, timeout);
}

PropertySet::Ptr EventSystem::matchingReceive(const std::string& topicName, const std::string& name, double value, const long timeout) {
    return _matchingReceive(topicName, name, value, timeout);
}

PropertySet::Ptr EventSystem::matchingReceive(const std::string& topicName, const std::string& name, float value, const long timeout) {
    return _matchingReceive(topicName, name, value, timeout);
}

PropertySet::Ptr EventSystem::matchingReceive(const std::string& topicName, const std::string& name, long long value, const long timeout) {
    return _matchingReceive(topicName, name, value, timeout);
}

PropertySet::Ptr EventSystem::matchingReceive(const std::string& topicName, const std::string& name, const std::string& value, const long timeout) {
    return _matchingReceive(topicName, name, value, timeout);
}

/** \brief Receives events matching both the name and string value within
  *        the specified timeout (in milliseconds). Waits until the
  *        matching event arrives, or until the timeout expires
  *  
  * \param topicName the topic to receive messages on
  * \param name the DataProperty name to match  
  * \param value the string value to match
  * \param timeout the time to wait (in milliseconds)
  * \return the matching PropertySet::Ptr object, 
  *         or a null PropertySet::Ptr object on timeout expiration
  */
template <typename T>
PropertySet::Ptr EventSystem::_matchingReceive(const std::string& topicName, const std::string& name, const T& value, long timeout) {
    boost::shared_ptr<EventReceiver> receiver;
    if ((receiver = getReceiver(topicName)) == 0) {
    throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "Topic "+ topicName +" is not registered with EventSystem");
    }
    return receiver->matchingReceive(name, value, timeout);
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
