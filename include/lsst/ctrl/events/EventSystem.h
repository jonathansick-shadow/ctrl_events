// -*- lsst-c++ -*-
/** \file EventSystem.h
  *
  * \ingroup events
  *
  * \brief defines the EventSystem class
  *
  * \author Stephen Pietrowicz, NCSA
  */

#ifndef LSST_CTRL_EVENTS_EVENTSYSTEM_H
#define LSST_CTRL_EVENTS_EVENTSYSTEM_H

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>

#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/logging/LogRecord.h"
#include "lsst/ctrl/events.h"

using lsst::daf::base::PropertySet;
namespace pexPolicy = lsst::pex::policy;

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

    void createTransmitter(const pexPolicy::Policy& policy);
    void createTransmitter(const std::string& hostName, const std::string& topicName);
    void createTransmitter(const std::string& hostName, const int hostPort, const std::string& topicName);


    void createReceiver(const pexPolicy::Policy& policy);
    void createReceiver(const std::string& hostName, const std::string& topicName);
    void createReceiver(const std::string& hostName, const int hostPort, const std::string& topicName);


    void publish(const std::string& topicName, const PropertySet::Ptr psp);
    void publish(const std::string& topicName, const pexLogging::LogRecord& rec);

    // void publishEvent(const std::string& topicName, const Event& event);

    PropertySet::Ptr receive(const std::string& topicName);
    PropertySet::Ptr receive(const std::string& topicName, const long timeout);

    // Event* receiveEvent(const std::string& topicName);
    // Event* receiveEvent(const std::string& topicName, const long timeout);

    static const int DEFAULTHOSTPORT = 61616;
private:
    boost::shared_ptr<EventTransmitter> getTransmitter(const std::string& name);
    boost::shared_ptr<EventReceiver> getReceiver(const std::string& name);

protected:
    static EventSystem *defaultEventSystem;
    list<boost::shared_ptr<EventTransmitter> >_transmitters;
    list<boost::shared_ptr<EventReceiver> >_receivers;
};
}
}
}

#endif /*end LSST_CTRL_EVENTS_EVENTSYSTEM_H*/

