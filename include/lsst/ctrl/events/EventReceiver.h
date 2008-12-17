// -*- lsst-c++ -*-
/** \class EventReceiver
  *
  * \ingroup events
  *
  * \brief Receive events from the event bus
  *
  * \author Stephen Pietrowicz, NCSA
  *
  */

#ifndef EVENTRECEIVER_H_
#define EVENTRECEIVER_H_

#include <activemq/concurrent/Thread.h>
#include <activemq/concurrent/Runnable.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/Integer.h>
#include <activemq/util/Config.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#ifdef REMOVE_THIS
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#endif
#include <cms/MapMessage.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"


namespace dafBase = lsst::daf::base;

namespace pexPolicy = lsst::pex::policy;

using namespace std;

namespace lsst {
namespace ctrl {
namespace events { 

class EventReceiver {
public:
    EventReceiver(const pexPolicy::Policy& policy);

    EventReceiver(const std::string& hostName, const std::string& topicName);

    // virtual destructor
    virtual ~EventReceiver();

    dafBase::PropertySet::Ptr receive();

    dafBase::PropertySet::Ptr receive(long timeout);

    // TODO: make thesetemplated methods as soon as I figure out how to SWIG them properly...
    // template <typename T> dafBase::PropertySet::Ptr matchingReceive(const std::string& name, const T& value);
    // template <typename T> dafBase::PropertySet::Ptr matchingReceive(const std::string& name, const T& value, long timeout);

    dafBase::PropertySet::Ptr matchingReceive(const std::string& name, long value);
    dafBase::PropertySet::Ptr matchingReceive(const std::string& name, int value);
    dafBase::PropertySet::Ptr matchingReceive(const std::string& name, float value);
    dafBase::PropertySet::Ptr matchingReceive(const std::string& name, double value);
    dafBase::PropertySet::Ptr matchingReceive(const std::string& name, long long value);
    dafBase::PropertySet::Ptr matchingReceive(const std::string& name, const std::string& value);

    dafBase::PropertySet::Ptr matchingReceive(const std::string& name, int value, long timeout);
    dafBase::PropertySet::Ptr matchingReceive(const std::string& name, long value, long timeout);
    dafBase::PropertySet::Ptr matchingReceive(const std::string& name, float value, long timeout);
    dafBase::PropertySet::Ptr matchingReceive(const std::string& name, double value, long timeout);
    dafBase::PropertySet::Ptr matchingReceive(const std::string& name, long long value, long timeout);
    dafBase::PropertySet::Ptr matchingReceive(const std::string& name, const std::string& value, long timeout);

    std::string getTopicName();

    static const long infiniteTimeout = -1;

private:
    void init(const std::string& hostName, const std::string& topicName);
    dafBase::PropertySet::Ptr _receive();
    dafBase::PropertySet::Ptr _receive(long timeout);
    template <typename T> dafBase::PropertySet::Ptr _matchingReceive(const std::string& name, const T& value);
    template <typename T> dafBase::PropertySet::Ptr _matchingReceiveTimeout(const std::string& name, const T& value, long timeout);

    dafBase::PropertySet::Ptr processStandaloneMessage(int sock);
    dafBase::PropertySet::Ptr unmarshall(const std::string& text);

    // connection to the JMS broker
    cms::Connection* _connection;

    // JMS session
    cms::Session* _session;

    // JMS message destination
    cms::Destination* _destination;

    // Object that receives the messages
    cms::MessageConsumer* _consumer;

    dafBase::PropertySet::Ptr processTextMessage(const cms::TextMessage* textMessage);

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

    bool matches(const dafBase::PropertySet::Ptr& psp, const std::string& name, boost::any value);

    dafBase::PropertySet::Ptr checkMessageCache(const std::string& name,
                                            boost::any value);

    // this is the simple "cache" used to retain events that weren't matched,
    // but still need to be delivered.
    list<dafBase::PropertySet::Ptr>_messageCache;

};


}
}
}


#endif /*EVENTRECEIVER_H_*/
