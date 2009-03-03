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

#include "lsst/ctrl/events/EventTransmitter.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/exceptions.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/logging/LogRecord.h"
#include <sys/socket.h>
#include <sys/un.h>

namespace pexExceptions = lsst::pex::exceptions;
namespace pexLogging = lsst::pex::logging;

using namespace std;

namespace lsst {
namespace ctrl {
namespace events {

/** \brief Configures an EventTransmitter based on Policy contents.
  *
  * The Policy object is checked for four keywords:
  *
  * topicName - the name of the topic to send to       
  * useLocalSockets - use UNIX domain sockets, instead of the JMS daemon
  * hostname - the name of the host hosting the JMS daemon
  * turnEventsOff - turn off event transmission
  *
  * Defaults are:
  *
  * useLocalSockets = false
  * turnEventsOff = false
  * 
  * The dependencies for these keywords are as follows:
  *
  * 1) If turnEventsOff is specified as true, no further checking is done, and 
  * no events will be transmitted.
  *
  * 2) If no topicName is specified, a NotFound exception is thrown
  *
  * 3) If useLocalSockets is false, and no hostName is specified, a
  * NotFound exception is thrown
  *
  * \param policy the policy object to use when building the receiver
  * \throw throws NotFoundException if expected keywords are missing in Policy object
  * \throw throws RuntimeErrorException if connection to transport mechanism fails
  */
EventTransmitter::EventTransmitter( const pexPolicy::Policy& policy) {
    _turnEventsOff = policy.getBool("turnEventsoff", false);
    if (_turnEventsOff == true)
        return;

    if (!policy.exists("topicName")) {
        throw LSST_EXCEPT(pexExceptions::NotFoundException, "topicName not found in policy");
    }

    _useLocalSockets = policy.getBool("useLocalSockets", false);
    if (_useLocalSockets == false) {
        if (!policy.exists("hostName")) {
            throw LSST_EXCEPT(pexExceptions::NotFoundException, "hostname must be specified with 'useLocalSockets' is false");
        }
    }
    init(policy.getString("hostName", ""), policy.getString("topicName"));
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
    _useLocalSockets = false;
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

    // If the _useLocalSockets flag is set, create a Unix domain
    // socket using the topic name to transmit events

    if (_useLocalSockets == true) {
        int len;
        struct sockaddr_un remote;

        _sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (_sock == -1) {
            throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "failed to open local socket");
        }
        remote.sun_family = AF_UNIX;
        std::string unix_socket = "/tmp/"+_topic;
        strcpy(remote.sun_path, unix_socket.c_str());
        len = strlen(remote.sun_path)+sizeof(remote.sun_family)+1;

        if (connect(_sock, (struct sockaddr *)&remote, len) == -1) {
            throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "could not connect to local socket");
        }

        return;
    }

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

/** private method used to send event out on the wire.
  */
void EventTransmitter::publish(const std::string& type, const PropertySet& ps) {
    // send the marshalled message to the local socket

    if (_useLocalSockets == true) {
        std::string str = marshall(ps);
        int str_len = strlen(str.c_str());

        // length of buffer
        if (send(_sock, &str_len, 4, 0) == -1) {
            throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "couldn't send message on local socket");
        }

        // buffer
        if (send(_sock, str.c_str(), str_len, 0) == -1) {
            throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "couldn't send message on local socket");
        }
        return;
    }

    if (_session == 0)
        throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "Not connected to event server");

    // Sending the message to the ActiveMQ server
    cms::TextMessage* message = _session->createTextMessage();
    message->setCMSType(type);
    std::string payload = marshall(ps);

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
            for (iter = vec.begin(); iter != vec.end(); iter++) {
                payload << "float||"<< name << "||"<< *iter << "~~";
            }
        } else if (ps.typeOf(name) == typeid(double)) {
            std::vector<double> vec  = ps.getArray<double>(name);
            std::vector<double>::iterator iter;
            for (iter = vec.begin(); iter != vec.end(); iter++) {
                payload << "double||"<< name << "||"<< *iter << "~~";
            }
        } else if (ps.typeOf(name) == typeid(std::string)) {
            std::vector<std::string> vec  = ps.getArray<std::string>(name);
            std::vector<std::string>::iterator iter;
            for (iter = vec.begin(); iter != vec.end(); iter++) {
                payload << "string||" << name << "||"<< *iter << "~~";
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

    if (_useLocalSockets == true) {
        close(_sock);
        return;
    }

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
