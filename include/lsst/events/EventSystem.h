// -*- lsst-c++ -*-
/** \class EventSystem
  *
  * \ingroup events
  *
  * \brief Coordinate publishing and receiving events
  *
  * \author Stephen Pietrowicz, NCSA
  */

#ifndef LSST_EVENTS_EVENTSYSTEM_H_
#define LSST_EVENTS_EVENTSYSTEM_H_

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <list>
#include <boost/shared_ptr.hpp>

#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/DataProperty.h"
#include "lsst/pex/logging/LogRecord.h"
#include "lsst/ctrl/events/Events.h"

using lsst::daf::base::DataProperty;
using lsst::pex::logging::LogRecord;

using namespace lsst::daf::base;
using namespace lsst::pex::policy;
using namespace std;

namespace lsst {
namespace events {
class EventSystem {
public:
    EventSystem();

    ~EventSystem();

    static const EventSystem& getDefaultEventSystem();

    void createTransmitter(const std::string& hostName, 
                           const std::string& topicName);

    void createTransmitter(const Policy& policy);

    void createLocalTransmitter(const std::string& topicName);

    void createReceiver(const std::string& hostName, 
                        const std::string& topicName);

    void createReceiver(const Policy& policy);

    void createLocalReceiver(const std::string& topicName);

    void publish(const std::string& topicName, const DataProperty::PtrType dpt);

    void publish(const std::string& topicName, const LogRecord& rec);

    DataProperty::PtrType receive(const std::string& topicName);

    DataProperty::PtrType receive(const std::string& topicName,
                                  const long timeout);

    DataProperty::PtrType matchingReceive(const std::string& topicName,
                                          const std::string& name,
                                          const std::string& value);

    DataProperty::PtrType matchingReceive(const std::string& topicName,
                                          const std::string& name, 
                                          int value);

    DataProperty::PtrType matchingReceive(const std::string& topicName,
                                          const std::string& name,
                                          float value);

    DataProperty::PtrType matchingReceive(const std::string& topicName, 
                                          const std::string& name,
                                          double value);

    DataProperty::PtrType matchingReceive(const std::string& topicName, 
                                          const std::string& name, 
                                          boost::any value);

    DataProperty::PtrType matchingReceive(const std::string& topicName,
                                          const std::string& name,
                                          const std::string& value,
                                          long timeout);

    DataProperty::PtrType matchingReceive(const std::string& topicName,
                                          const std::string& name, 
                                          int value,
                                          long timeout);

    DataProperty::PtrType matchingReceive(const std::string& topicName,
                                          const std::string& name,
                                          float value,
                                          long timeout);

    DataProperty::PtrType matchingReceive(const std::string& topicName,
                                          const std::string& name,
                                          double value,
                                          long timeout);

    DataProperty::PtrType matchingReceive(const std::string& topicName,
                                          const std::string& name,
                                          boost::any value,
                                          long timeout);

private:
    shared_ptr<EventTransmitter> getTransmitter(const std::string& name);
    shared_ptr<EventReceiver> getReceiver(const std::string& name);

protected:
    static EventSystem *defaultEventSystem;
    list<shared_ptr<EventTransmitter> >_transmitters;
    list<shared_ptr<EventReceiver> >_receivers;
};
}
}

#endif /*LSST_EVENTS_EVENTSYSTEM_H_*/

