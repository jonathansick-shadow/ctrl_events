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
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/pex/logging/LogRecord.h"
#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/DataProperty.h"

using lsst::daf::base::DataProperty;
using lsst::pex::logging::LogRecord;

using namespace lsst::daf::base;
using namespace lsst::pex::policy;
using namespace activemq;
using namespace activemq::core;
using namespace activemq::util;
using namespace activemq::concurrent;
using namespace cms;
using namespace std;

namespace lsst {
namespace ctrl {
namespace events { 

class EventReceiver {
public:
    EventReceiver(const Policy& policy);

    EventReceiver(const std::string& hostName, const std::string& topicName);

    // virtual destructor
    virtual ~EventReceiver();

    DataProperty::PtrType receive();

    DataProperty::PtrType receive(long timeout);

    DataProperty::PtrType matchingReceive(const std::string& name, const std::string& value);

    DataProperty::PtrType matchingReceive(const std::string& name, int value);

    DataProperty::PtrType matchingReceive(const std::string& name, float value);

    DataProperty::PtrType matchingReceive(const std::string& name, double value);

    DataProperty::PtrType matchingReceive(const std::string& name, const std::string& value, long timeout);

    DataProperty::PtrType matchingReceive(const std::string& name, int value, long timeout);

    DataProperty::PtrType matchingReceive(const std::string& name, float value, long timeout);

    DataProperty::PtrType matchingReceive(const std::string& name, double value, long timeout);

    DataProperty::PtrType matchingReceive(const std::string& name, boost::any value);
    DataProperty::PtrType matchingReceive(const std::string& name, boost::any value, long timeout);

    std::string getTopicName();

    static const long infiniteTimeout = -1;

private:
    void init(const std::string& hostName, const std::string& topicName);
    DataProperty::PtrType _receive();
    DataProperty::PtrType _receive(long timeout);

    DataProperty::PtrType processStandaloneMessage(int sock);
    DataProperty::PtrType unmarshall(int tuples, std::string text);
    DataProperty::PtrType unmarshall(int tuples, DataProperty::PtrType root, std::vector<string> vec, int& pos);

    // connection to the JMS broker
    Connection* _connection;

    // JMS session
    Session* _session;

    // JMS message destination
    Destination* _destination;

    // Object that receives the messages
    MessageConsumer* _consumer;

    DataProperty::PtrType processTextMessage(const TextMessage* textMessage);

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

    bool equal(boost::any v1, boost::any v2);

    DataProperty::PtrType checkMessageCache(const std::string& name,
                                            boost::any value);

    // this is the simple "cache" used to retain events that weren't matched,
    // but still need to be delivered.
    list<DataProperty::PtrType>_messageCache;

};


}
}
}


#endif /*EVENTRECEIVER_H_*/
