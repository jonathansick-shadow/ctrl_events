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
#ifdef REMOVE_THIS
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#endif
#include <cms/MapMessage.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/pex/policy.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"

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

    // virtual destructor
    virtual ~EventReceiver();

    PropertySet::Ptr receive();

    PropertySet::Ptr receive(long timeout);

    // TODO: make thesetemplated methods as soon as I figure out how to SWIG them properly...
    // template <typename T> PropertySet::Ptr matchingReceive(const std::string& name, const T& value);
    // template <typename T> PropertySet::Ptr matchingReceive(const std::string& name, const T& value, long timeout);

    PropertySet::Ptr matchingReceive(const std::string& name, long value);
    PropertySet::Ptr matchingReceive(const std::string& name, int value);
    PropertySet::Ptr matchingReceive(const std::string& name, float value);
    PropertySet::Ptr matchingReceive(const std::string& name, double value);
    PropertySet::Ptr matchingReceive(const std::string& name, long long value);
    PropertySet::Ptr matchingReceive(const std::string& name, const std::string& value);

    PropertySet::Ptr matchingReceive(const std::string& name, int value, long timeout);
    PropertySet::Ptr matchingReceive(const std::string& name, long value, long timeout);
    PropertySet::Ptr matchingReceive(const std::string& name, float value, long timeout);
    PropertySet::Ptr matchingReceive(const std::string& name, double value, long timeout);
    PropertySet::Ptr matchingReceive(const std::string& name, long long value, long timeout);
    PropertySet::Ptr matchingReceive(const std::string& name, const std::string& value, long timeout);

    std::string getTopicName();

    static const long infiniteTimeout = -1;

private:
    void init(const std::string& hostName, const std::string& topicName);
    PropertySet::Ptr _receive();
    PropertySet::Ptr _receive(long timeout);
    template <typename T> PropertySet::Ptr _matchingReceive(const std::string& name, const T& value);
    template <typename T> PropertySet::Ptr _matchingReceiveTimeout(const std::string& name, const T& value, long timeout);

    PropertySet::Ptr processStandaloneMessage(int sock);
    PropertySet::Ptr unmarshall(const std::string& text);

    // connection to the JMS broker
    cms::Connection* _connection;

    // JMS session
    cms::Session* _session;

    // JMS message destination
    cms::Destination* _destination;

    // Object that receives the messages
    cms::MessageConsumer* _consumer;

    PropertySet::Ptr processTextMessage(const cms::TextMessage* textMessage);

    void splitString(std::string str, std::string delim, std::vector<std::string>&results);

    // used to indicate "standalone mode", running without using the 
    // ActiveMQ server
    bool _useLocalSockets;

    // used to completely turn off event  transmission
    bool _turnEventsOff;

    // socket for "standalone mode"
    int _sock;

    // the topic for this receiver
    std::string _topic;

    bool matches(const PropertySet::Ptr& psp, const std::string& name, boost::any value);

    PropertySet::Ptr checkMessageCache(const std::string& name,
                                            boost::any value);

    // this is the simple "cache" used to retain events that weren't matched,
    // but still need to be delivered.
    list<PropertySet::Ptr>_messageCache;

};


}
}
}


#endif /*end LSST_CTRL_EVENTS_EVENTRECEIVER_H*/
