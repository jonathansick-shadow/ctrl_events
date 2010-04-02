// -*- lsst-c++ -*-
/** \file EventReceiver.h
  *
  * \ingroup events
  *
  * \brief defines the EventReceiver class
  *
  * \author Stephen Pietrowicz, NCSA
  *
  */

#ifndef LSST_CTRL_EVENTS_EVENTRECEIVER_H
#define LSST_CTRL_EVENTS_EVENTRECEIVER_H

#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/pex/policy.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/ctrl/events/Event.h"

using lsst::daf::base::PropertySet;

namespace pexPolicy = lsst::pex::policy;

using namespace std;

namespace lsst {
namespace ctrl {
namespace events { 
/**
 * @brief Receive events from the event bus
 */
class EventReceiver {
public:
    EventReceiver(const pexPolicy::Policy& policy);
    EventReceiver(const std::string& hostName, const std::string& topicName);
    EventReceiver(const std::string& hostName, const std::string& topicName, const std::string& selector);
    EventReceiver(const std::string& hostName, const int hostPort, const std::string& topicName);
    EventReceiver(const std::string& hostName, const int hostPort, const std::string& topicName, const std::string& selector);

    // virtual destructor
    virtual ~EventReceiver();

    PropertySet::Ptr receive();
    PropertySet::Ptr receive(long timeout);

    Event* receiveEvent();
    Event* receiveEvent(long timeout);

    std::string getTopicName();

    static const long infiniteTimeout = -1;

private:
    void init(const std::string& hostName, const int port, const std::string& topicName, const std::string& selector);
    PropertySet::Ptr _receive(long timeout);

    PropertySet::Ptr unmarshall(const std::string& text);

    // connection to the JMS broker
    cms::Connection* _connection;

    // JMS session
    cms::Session* _session;

    // JMS message destination
    cms::Destination* _destination;

    // Object that receives the messages
    cms::MessageConsumer* _consumer;

    PropertySet::Ptr processTextMessage(cms::TextMessage* textMessage);

    void splitString(std::string str, std::string delim, std::vector<std::string>&results);

    // used to completely turn off event  transmission
    bool _turnEventsOff;

    // the topic for this receiver
    std::string _topic;

    // the selector for this receiver
    std::string _selector;

};


}
}
}


#endif /*end LSST_CTRL_EVENTS_EVENTRECEIVER_H*/
