// -*- lsst-c++ -*-
/** \file EventTransmitter.cc
  *
  * \brief Objects to send Events to the specified event bus
  *
  * \ingroup ctrl/events
  *
  * \author Stephen R. Pietrowicz, NCSA
  *
  */
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <cstring>

#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/EventTransmitter.h"
#include "lsst/daf/base/DateTime.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/exceptions.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/logging/LogRecord.h"
#include <sys/socket.h>
#include <sys/un.h>

#include <activemq/exceptions/ActiveMQException.h>
#include <activemq/core/ActiveMQConnectionFactory.h>

namespace pexExceptions = lsst::pex::exceptions;
namespace pexLogging = lsst::pex::logging;


using namespace std;
using std::numeric_limits;

namespace lsst {
namespace ctrl {
namespace events {

/** \brief Configures an EventTransmitter based on Policy contents.
  *
  * The Policy object is checked for four keywords:
  *
  * topicName - the name of the topic to send to       
  * hostname - the name of the host hosting the JMS daemon
  * turnEventsOff - turn off event transmission
  *
  * Defaults are:
  *
  * turnEventsOff = false
  * 
  * The dependencies for these keywords are as follows:
  *
  * 1) If turnEventsOff is specified as true, no further checking is done, and 
  * no events will be transmitted.
  *
  * 2) If no topicName is specified, a NotFound exception is thrown
  *
  * \param policy the policy object to use when building the receiver
  * \throw throws NotFoundException if expected keywords are missing in Policy object
  * \throw throws RuntimeErrorException if connection to transport mechanism fails
  */
EventTransmitter::EventTransmitter( const pexPolicy::Policy& policy) {

    try {
        _turnEventsOff = policy.getBool("turnEventsoff");
    } catch (pexPolicy::NameNotFound& e) {
        _turnEventsOff = false;
    }
    if (_turnEventsOff == true)
        return;

    if (!policy.exists("topicName")) {
        throw LSST_EXCEPT(pexExceptions::NotFoundException, "topicName not found in policy");
    }

    std::string hostName;
    try {
        hostName = policy.getString("hostName");
    } catch (pexPolicy::NameNotFound& e) {
        throw LSST_EXCEPT(pexExceptions::NotFoundException, "hostName not found in policy");
    }
    init(hostName, policy.getString("topicName"));
}

/** \brief Transmits events to the specified host and topic
  *
  * \param hostName the machine hosting the message broker
  * \param topicName the topic to transmit events to
  * \throw throws RuntimeErrorException if local socket can't be created
  * \throw throws RuntimeErrorException if connect to local socket fails
  * \throw throws RuntimeErrorException if connect to remote ActiveMQ host fails
  */
EventTransmitter::EventTransmitter( const std::string& hostName,
                                    const std::string& topicName) {
    _turnEventsOff = false;
    init(hostName, topicName);
}


/** private initialization method for configuring EventTransmitter
  */
void EventTransmitter::init( const std::string& hostName,
                                    const std::string& topicName) {

    _connection = NULL;
    _session = NULL;
    _destination = NULL;
    _producer = NULL;
    _topic = topicName;

    if (_turnEventsOff == true)
        return;

    // set up a connection to the ActiveMQ server for message transmission
    try {

        /*
         * Create a ConnectionFactory to connect to hostName, and
         * create a topic for this.
         */
        string brokerUri = "tcp://"+hostName+
                      ":61616?wireFormat=openwire&transport.useAsyncSend=true";

        activemq::core::ActiveMQConnectionFactory* connectionFactory =
            new activemq::core::ActiveMQConnectionFactory( brokerUri );

        _connection = connectionFactory->createConnection();
        _connection->start();

        delete connectionFactory;

        _session = _connection->createSession( cms::Session::AUTO_ACKNOWLEDGE );

        // Create the destination topic
        _destination = _session->createTopic( topicName );

        // Create a MessageProducer from the Session to the Topic or Queue
        _producer = _session->createProducer( _destination );
        _producer->setDeliveryMode( cms::DeliveryMode::NON_PERSISTENT );
    } catch ( cms::CMSException& e ) {
        throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, e.getMessage());
    }
}

void EventTransmitter::publish(const PropertySet::Ptr& psp) {
    publish(*psp);
}

void EventTransmitter::publish(const PropertySet& ps) {
    if (_turnEventsOff == true)
        return;

    publish("event", ps);
}

/** \brief publish an event of "type"        
  *
  * \param rec The LogRecord to send.  This is used internally by the logging
  *            subsystem and is exposed here to send LogRecord through event
  *            channels.
  * \throw throws Runtime exception if publish fails
  */
void EventTransmitter::publish(const pexLogging::LogRecord& rec) {

    if (_turnEventsOff == true)
        return;

    const PropertySet& ps = rec.getProperties();

    publish("logging", ps);
}

void EventTransmitter::publish(Event& event) {
    if (_session == 0)
        throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "Not connected to event server");

    std::ostringstream payload;

    // Sending the message to the ActiveMQ server
    cms::TextMessage* message = _session->createTextMessage();

    message->setCMSType(event.getType());

    message->setStringProperty("hostId",event.getHostId());
    message->setStringProperty("runId",event.getRunId());
    message->setStringProperty("status",event.getStatus());
    message->setStringProperty("topic",_topic);
    message->setLongProperty("eventTime",getCurrentTime());

    // delete marshall line below
    // std::string payload = marshall(ps);

    PropertySet::Ptr psp = event.getPropertySet();

    if (psp != 0) {
        std::vector<std::string> v = psp->paramNames(false);
        unsigned int i;
        // XXX - there's probably a way to get the first element, not the array.  fix this.
        for (i = 0; i < v.size(); i++) {
            std::string name = v[i];
            if (psp->typeOf(name) == typeid(std::string)) {
                /*
                std::vector<std::string> vec  = psp->getArray<std::string>(name);
                std::vector<std::string>::iterator iter;
                iter = vec.begin();
                message->setStringProperty(name, *iter);
                */
                std::string data = psp->get<std::string>(name);
                message->setStringProperty(name, data);
                payload << "string|"<< name << ";";
            } else if (psp->typeOf(name) == typeid(bool)) {
                bool data = psp->get<bool>(name);
                message->setBooleanProperty(name, data);
                payload << "bool|"<< name << ";";
            } else if (psp->typeOf(name) == typeid(unsigned char)) {
                unsigned char data = psp->get<unsigned char>(name);
                message->setByteProperty(name, data);
                payload << "unsigned char|"<< name << ";";
            } else if (psp->typeOf(name) == typeid(int)) {
                int data = psp->get<int>(name);
                message->setIntProperty(name, data);
                payload << "int|"<< name << ";";
            } else if (psp->typeOf(name) == typeid(float)) {
                float data = psp->get<float>(name);
                message->setFloatProperty(name, data);
                payload << "float|"<< name << ";";
            } else if (psp->typeOf(name) == typeid(long long)) {
                long long data = psp->get<long long>(name);
                message->setLongProperty(name, data);
                payload << "long long|"<< name << ";";
            } else if (psp->typeOf(name) == typeid(short)) {
                short data = psp->get<short>(name);
                message->setShortProperty(name, data);
                payload << "short|"<< name << ";";
            } else if (psp->typeOf(name) == typeid(double)) {
                double data = psp->get<double>(name);
                message->setDoubleProperty(name, data);
                payload << "double|"<< name << ";";
            }
        }

    }
}

long EventTransmitter::getCurrentTime() {
    struct timeval tv;
    long currentTime;

    gettimeofday(&tv,0);
    currentTime = (tv.tv_sec *1000000 + tv.tv_usec)*1000;
    return currentTime;
}



/** private method used to send event out on the wire.
  */
void EventTransmitter::publish(const std::string& type, const PropertySet& ps) {

    if (_session == 0)
        throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "Not connected to event server");

    // Sending the message to the ActiveMQ server
    cms::TextMessage* message = _session->createTextMessage();
    message->setCMSType(type);
    std::string payload = marshall(ps);

#ifdef NOTDEF
    // add the message header, if there is one.
    if (_header != 0) {
        std::vector<std::string> v = _header->paramNames(false);
        unsigned int i;
        // XXX - there's probably a way to get the first element, not the array.  fix this.
        for (i = 0; i < v.size(); i++) {
            std::string name = v[i];
            if (_header->typeOf(name) == typeid(std::string)) {
                std::vector<std::string> vec  = _header->getArray<std::string>(name);
                std::vector<std::string>::iterator iter;
                iter = vec.begin();
                message->setStringProperty(name, *iter);
            } else if (_header->typeOf(name) == typeid(bool)) {
                std::vector<bool> vec  = _header->getArray<bool>(name);
                std::vector<bool>::iterator iter;
                iter = vec.begin();
                // msg->setBooleanProperty(name, *iter);
            } else if (_header->typeOf(name) == typeid(int)) {
                std::vector<int> vec  = _header->getArray<int>(name);
                std::vector<int>::iterator iter;
                iter = vec.begin();
                // msg->setIntProperty(name, *iter);
            } else if (_header->typeOf(name) == typeid(float)) {
                std::vector<float> vec  = _header->getArray<float>(name);
                std::vector<float>::iterator iter;
                iter = vec.begin();
                // msg->setFloatProperty(name, *iter);
            } else if (_header->typeOf(name) == typeid(double)) {
                std::vector<double> vec  = _header->getArray<double>(name);
                std::vector<double>::iterator iter;
                iter = vec.begin();
                // msg->setDoubleProperty(name, *iter);
            }
        }

    }
#endif

    message->setText(payload);
    _producer->send( message );

    delete message;
}

std::string EventTransmitter::marshall(const PropertySet& ps) {
    std::vector<std::string> v = ps.paramNames(false);

    // TODO: optimize this to get use getArray only when necessary
    std::ostringstream payload;
    unsigned int i;
    payload << "nodelist||nodelist||" << (v.size()-1) << "~~";
    for (i = 0; i < v.size(); i++) {
        std::string name = v[i];
        if (ps.typeOf(name) == typeid(bool)) {
            std::vector<bool> vec  = ps.getArray<bool>(name);
            std::vector<bool>::iterator iter;
            for (iter = vec.begin(); iter != vec.end(); iter++) {
                payload << "bool||"<< name << "||" << *iter << "~~";
            }
        } else if (ps.typeOf(name) == typeid(int)) {
            std::vector<int> vec  = ps.getArray<int>(name);
            std::vector<int>::iterator iter;
            for (iter = vec.begin(); iter != vec.end(); iter++) {
                payload << "int||" << name << "||"<< *iter << "~~";
            }
        } else if (ps.typeOf(name) == typeid(float)) {
            std::vector<float> vec  = ps.getArray<float>(name);
            std::vector<float>::iterator iter;
            payload.precision(numeric_limits<float>::digits10+1);
            for (iter = vec.begin(); iter != vec.end(); iter++) {
                payload << "float||"<< name << "||"<< *iter << "~~";
            }
        } else if (ps.typeOf(name) == typeid(double)) {
            std::vector<double> vec  = ps.getArray<double>(name);
            std::vector<double>::iterator iter;
            payload.precision(numeric_limits<double>::digits10+1);
            for (iter = vec.begin(); iter != vec.end(); iter++) {
                payload << "double||"<< name << "||"<< *iter << "~~";
            }
        } else if (ps.typeOf(name) == typeid(std::string)) {
            std::vector<std::string> vec  = ps.getArray<std::string>(name);
            std::vector<std::string>::iterator iter;
            for (iter = vec.begin(); iter != vec.end(); iter++) {
                payload << "string||" << name << "||"<< *iter << "~~";
            }
        } else if (ps.typeOf(name) == typeid(lsst::daf::base::DateTime)) {
            std::vector<lsst::daf::base::DateTime> vec  = ps.getArray<lsst::daf::base::DateTime>(name);
            std::vector<lsst::daf::base::DateTime>::iterator iter;
            for (iter = vec.begin(); iter != vec.end(); iter++) {
                payload << "datetime||" << name << "||"<< (*iter).nsecs() << "~~";
            }
        }
    }
    return payload.str();
}

/** \brief get the topic name of this EventTransmitter
  */
std::string EventTransmitter::getTopicName() {
    return _topic;
}

/** \brief Destructor for EventTransmitter
  */
EventTransmitter::~EventTransmitter() {

    // Destroy resources.
    try {
        if( _destination != NULL )
            delete _destination;
    } catch ( cms::CMSException& e ) {
        e.printStackTrace();
    }
    _destination = NULL;

    try {
        if( _producer != NULL )
            delete _producer;
    } catch ( cms::CMSException& e ) {
        e.printStackTrace();
    }
    _producer = NULL;

    // Close open resources.
    try {
        if( _session != NULL )
            _session->close();
        if( _connection != NULL )
            _connection->close();
    } catch ( cms::CMSException& e ) {
        e.printStackTrace();
    }

    try {
        if( _session != NULL )
            delete _session;
    } catch ( cms::CMSException& e ) {
        e.printStackTrace();
    }
    _session = NULL;

    try {
        if( _connection != NULL )
            delete _connection;
    } catch ( cms::CMSException& e ) {
        e.printStackTrace();
    }
    _connection = NULL;
}

}
}
}
