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

    void createTransmitter(const std::string& hostName, 
                           const std::string& topicName);

    void createTransmitter(const std::string& hostName, 
                           const std::string& topicName,
                           const PropertySet::Ptr& header);

    void createTransmitter(const pexPolicy::Policy& policy);

    void createLocalTransmitter(const std::string& topicName);

    void createReceiver(const std::string& hostName, 
                        const std::string& topicName);

    void createReceiver(const std::string& hostName, 
                        const std::string& topicName,
                        const std::string& selector);

    void createReceiver(const pexPolicy::Policy& policy);

    void createLocalReceiver(const std::string& topicName);

    void publish(const std::string& topicName, const PropertySet::Ptr psp);

    void publish(const std::string& topicName, const pexLogging::LogRecord& rec);

    PropertySet::Ptr receive(const std::string& topicName);

    PropertySet::Ptr receive(const std::string& topicName,
                                  const long timeout);


    // TODO: all these need to be eliminated once the SWIG incantation for templates is figured out
    PropertySet::Ptr matchingReceive(const std::string& topicName, const std::string& name, const int value);
    PropertySet::Ptr matchingReceive(const std::string& topicName, const std::string& name, const long value);
    PropertySet::Ptr matchingReceive(const std::string& topicName, const std::string& name, const float value);
    PropertySet::Ptr matchingReceive(const std::string& topicName, const std::string& name, const double value);
    PropertySet::Ptr matchingReceive(const std::string& topicName, const std::string& name, const long long value);
    PropertySet::Ptr matchingReceive(const std::string& topicName, const std::string& name, const std::string& value);

    PropertySet::Ptr matchingReceive(const std::string& topicName, const std::string& name, const int value, long timeout);
    PropertySet::Ptr matchingReceive(const std::string& topicName, const std::string& name, const long value, long timeout);
    PropertySet::Ptr matchingReceive(const std::string& topicName, const std::string& name, const float value, long timeout);
    PropertySet::Ptr matchingReceive(const std::string& topicName, const std::string& name, const double value, long timeout);
    PropertySet::Ptr matchingReceive(const std::string& topicName, const std::string& name, const long long value, long timeout);
    PropertySet::Ptr matchingReceive(const std::string& topicName, const std::string& name, const std::string& value, long timeout);

private:
    template <typename T>
    PropertySet::Ptr _matchingReceive(const std::string& topicName,
                                          const std::string& name,
                                          const T& value);

    template <typename T>
    PropertySet::Ptr _matchingReceive(const std::string& topicName,
                                          const std::string& name,
                                          const T& value,
                                          long timeout);
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

