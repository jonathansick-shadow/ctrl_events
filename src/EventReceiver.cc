// -*- lsst-c++ -*-
/** \file EventReceiver.cc
  *
  * \brief Object to receive Events from the specified event bus
  *
  * \ingroup events
  *
  * \author Stephen R. Pietrowicz, NCSA
  *
  */
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "lsst/ctrl/events/EventReceiver.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/exceptions.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/logging/LogRecord.h"
#include <sys/socket.h>
#include <sys/un.h>

namespace pexPolicy = lsst::pex::policy;
namespace pexExceptions = lsst::pex::exceptions;

namespace activemqCore = activemq::core;

namespace lsst {
namespace ctrl {
namespace events {

/** \brief Receives events based on Policy file contents
  *
  * \param policy the policy object to use when building the receiver
  * \throw throws NotFound exception if topicName isn't specified
  * \throw throws NotFound exception if hostName isn't specified
  * \throw throws Runtime exception if connection fails to initialize
  */

EventReceiver::EventReceiver(const pexPolicy::Policy& policy) {
    _turnEventsOff = policy.getBool("turnEventsOff", false);
    if (_turnEventsOff == true)
        return;
    if (!policy.exists("topicName")) {
        throw LSST_EXCEPT(pexExceptions::NotFoundException, "topicName not found in policy");
    }

    std::string topicName = policy.getString("topicName");
    _turnEventsOff = policy.getBool("turnEventsOff", false);
    _useLocalSockets = policy.getBool("useLocalSockets", false);
    if (_useLocalSockets == false) {
        if (!policy.exists("hostName")) {
            throw LSST_EXCEPT(pexExceptions::NotFoundException, "hostName was not specified in policy file");
        }
    }


    init(policy.getString("hostName", "non"), topicName);
}

/** \brief Receives events from the specified host and topic
  *
  * \param hostName the machine hosting the message broker
  * \param topicName the topic to receive events from
  * \throw throws Runtime exception if connection fails to initialize
  */
EventReceiver::EventReceiver(const std::string& hostName, const std::string& topicName) {
    _turnEventsOff = false;
    _useLocalSockets = false;
    init(hostName, topicName);
}


/** private method for initialization of EventReceiver.  Sets up use of local
  * sockets or activemq, depending on how the policy file was configured.  
  */
void EventReceiver::init(const std::string& hostName, const std::string& topicName) {

    _connection = NULL;
    _session = NULL;
    _destination = NULL;
    _consumer = NULL;
    _sock = 0;
    _topic = topicName;

    if (_turnEventsOff == true)
        return;

    if (_useLocalSockets == true) {
        struct sockaddr_un local;
        _useLocalSockets = true;
        _sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (_sock == -1) {
            throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "couldn't create local socket");
        }

        bzero(&local, sizeof(local));
        local.sun_family = AF_UNIX;

        std::string unix_socket = "/tmp/"+topicName;

        strcpy(local.sun_path,unix_socket.c_str());
        unlink(local.sun_path);

        int len = strlen(local.sun_path)+sizeof(local.sun_family)+1;
        bind(_sock, (struct sockaddr *)&local, len);
        listen(_sock, 5);
        return;
    }
    try {

        string jmsURL = "tcp://"+hostName+":61616?wireFormat=openwire";

        activemqCore::ActiveMQConnectionFactory* connectionFactory =
            new activemqCore::ActiveMQConnectionFactory( jmsURL );


        _connection = connectionFactory->createConnection();
        delete connectionFactory;
        _connection->start();

        _session = _connection->createSession( cms::Session::AUTO_ACKNOWLEDGE );

        _destination = _session->createTopic( topicName );

        _consumer = _session->createConsumer( _destination );

    } catch ( cms::CMSException& e ) {
        e.printStackTrace();
    }
}

/** \brief blocking receive for events.  Waits until an event
  *        is received for the topic specified in the constructor
  * \return a DataProperty::PtrType object
  * \throw throws Runtime exception if receive fails unexpectedly
  */
PropertySet::Ptr EventReceiver::receive() {
    if (_messageCache.size() != 0) {
        PropertySet::Ptr psp = _messageCache.front();
        if (psp.get() != 0) {
            _messageCache.remove(psp);
            return psp;
        }
    }
    return _receive();
}

PropertySet::Ptr EventReceiver::_receive() {
    if (_useLocalSockets == false) {
        const cms::TextMessage* textMessage =
            dynamic_cast<const cms::TextMessage* >(_consumer->receive());
            return processTextMessage(textMessage);
    } else {
        return _receive(infiniteTimeout);
    }
}


/** \brief blocking receive for events, with timeout (in milliseconds).  
  *        Waits until an event is received or until the timeout expires.
  * \return a DataProperty::PtrType object
  *         in receive()
  * \throw throws Runtime exception if receive fails unexpectedly
  */
PropertySet::Ptr EventReceiver::receive(long timeout) {
    if (_messageCache.size() != 0) {
        PropertySet::Ptr psp = _messageCache.front();
        if (psp.get() != 0) {
            _messageCache.remove(psp);
            return psp;
        }
    }
    return _receive(timeout);
}

PropertySet::Ptr EventReceiver::matchingReceive(const std::string& name, int value, long timeout) { return _matchingReceiveTimeout(name, value, timeout); }
PropertySet::Ptr EventReceiver::matchingReceive(const std::string& name, long value, long timeout) { return _matchingReceiveTimeout(name, value, timeout); }
PropertySet::Ptr EventReceiver::matchingReceive(const std::string& name, float value, long timeout) { return _matchingReceiveTimeout(name, value, timeout); }
PropertySet::Ptr EventReceiver::matchingReceive(const std::string& name, double value, long timeout) { return _matchingReceiveTimeout(name, value, timeout); }
PropertySet::Ptr EventReceiver::matchingReceive(const std::string& name, long long value, long timeout) { return _matchingReceiveTimeout(name, value, timeout); }
PropertySet::Ptr EventReceiver::matchingReceive(const std::string& name, const std::string& value, long timeout) { return _matchingReceiveTimeout(name, value, timeout); }

/** \brief Receives events matching both the name and string value, with timeout (in milliseconds
  *        Waits until a matching event is received or until the timeout expires.
  * \param name the DataProperty name to match
  * \param value the DataProperty value to match
  * \param timeout the time to wait (in milliseconds)
  * \return the matching DataProperty::PtrType object
  * \throw throws Runtime exception if receive fails unexpectedly
  */
template <typename T>
PropertySet::Ptr EventReceiver::_matchingReceiveTimeout(const std::string& name, const T& value, long timeout) {
    struct timeval tvStart;
    struct timezone tzStart;
    int sec, usec, total_usec, total_msec;
    int ret;

    PropertySet::Ptr psp = checkMessageCache(name, value);
    if (psp.get() != 0) {
        _messageCache.remove(psp);
        return psp;
    }

    /*
     * Receiving with timeout is slightly more complex when you're matching.  Messages
     * could come out of order, and if they do, you want to still wait for any time
     * you might have remaining to be sure the event you want doesn't come in during
     *  that time.
     */

    ret = gettimeofday(&tvStart, &tzStart);
    long currentTimeout = timeout;
    while (currentTimeout > 0) {
            psp = _receive(currentTimeout);
            if (psp.get() == 0) {
                return PropertySet::Ptr();
            }

            if (matches(psp, name, value) == true) {
                return psp;
            }

            _messageCache.push_back(psp);

            struct timeval tvCurrent;
            struct timezone tzCurrent;
            ret = gettimeofday(&tvCurrent, &tzCurrent);

            sec = tvCurrent.tv_sec-tvStart.tv_sec;
            usec = tvCurrent.tv_usec-tvStart.tv_usec;

            total_usec = sec*1000000+usec;
            total_msec = total_usec/1000;


            int timeLeft = timeout-total_msec;
            currentTimeout = timeLeft;
            if (currentTimeout <= 0) {
                return PropertySet::Ptr();
            }
    }
    return PropertySet::Ptr();
}

/** private method that performs a regular (non-matching) receive for all
  * variations of the public receive methods.
  */
PropertySet::Ptr EventReceiver::_receive(long timeout) {
    if (_turnEventsOff == true)
        return PropertySet::Ptr();

    if (_useLocalSockets == false) {
        const cms::TextMessage* textMessage =
            dynamic_cast<const cms::TextMessage* >(_consumer->receive(timeout));
            return processTextMessage(textMessage);
    } else {
        struct timeval tv;
        fd_set readfds;

        if (timeout >= 0) {
                tv.tv_sec = (int)(timeout/1000);
                tv.tv_usec = (int)((timeout-(tv.tv_sec*1000))*1000);
        }

        FD_ZERO(&readfds);
        FD_SET(_sock, &readfds);

        int select_val;
        if (timeout == infiniteTimeout) {
            select_val = select(_sock+1, &readfds, (fd_set *)0, (fd_set *)0, NULL);
        } else {
            select_val = select(_sock+1, &readfds, (fd_set *)0, (fd_set *)0, &tv);
        }
        if (select_val == 0) {
            return PropertySet::Ptr();
        } else if (select_val < 0) {
            throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "error on local socket select");
            // return DataProperty::PtrType();
        }

        if (FD_ISSET(_sock, &readfds)) {
            struct sockaddr_un remote;
            int len = sizeof(struct sockaddr_un);
            int remote_sock = accept(_sock, (struct sockaddr *)&remote, (socklen_t *)&len);
            if (remote_sock < 0)
                throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, "error on local socket accept");
            return processStandaloneMessage(remote_sock);
        }
       return PropertySet::Ptr();
    }
}
PropertySet::Ptr EventReceiver::matchingReceive(const std::string& name, int value) { return _matchingReceive(name, value); }
PropertySet::Ptr EventReceiver::matchingReceive(const std::string& name, long value) { return _matchingReceive(name, value); }
PropertySet::Ptr EventReceiver::matchingReceive(const std::string& name, float value) { return _matchingReceive(name, value); }
PropertySet::Ptr EventReceiver::matchingReceive(const std::string& name, double value) { return _matchingReceive(name, value); }
PropertySet::Ptr EventReceiver::matchingReceive(const std::string& name, long long value) { return _matchingReceive(name, value); }
PropertySet::Ptr EventReceiver::matchingReceive(const std::string& name, const std::string& value) { return _matchingReceive(name, value); }


template <typename T>
PropertySet::Ptr EventReceiver::_matchingReceive(const std::string& name, const T& value) {

    // check queue first
    PropertySet::Ptr psp = checkMessageCache(name, value);
    if (psp.get() != 0) {
        _messageCache.remove(psp);
        return psp;
    }
    while (true) { 
        psp = _receive();
        if (matches(psp, name, value) == true) {
                return psp;
        }
        _messageCache.push_back(psp);
    }
    return PropertySet::Ptr();
}

bool EventReceiver::matches(const PropertySet::Ptr& psp, const std::string& name, boost::any value) {
    if (psp->exists(name) == false)
        return false;
    if (psp->typeOf(name) == typeid(int)) {
        int v1 = boost::any_cast<int>(value);
        int v2 = psp->get<int>(name);
        return (v1 == v2);
    } else if (psp->typeOf(name) == typeid(std::string)) {
        std::string v1 = boost::any_cast<std::string>(value);
        std::string v2 = psp->get<std::string>(name);
        return (v1 == v2);
    } else if (psp->typeOf(name) == typeid(double)) {
        double v1 = boost::any_cast<double>(value);
        double v2 = psp->get<double>(name);
        return (v1 == v2);
    } else if (psp->typeOf(name) == typeid(long)) {
        long v1 = boost::any_cast<long>(value);
        long v2 = psp->get<long>(name);
        return (v1 == v2);
    } else if (psp->typeOf(name) == typeid(long long)) {
        long long v1 = boost::any_cast<long long>(value);
        long long v2 = psp->get<long long>(name);
        return (v1 == v2);
    } else if (psp->typeOf(name) == typeid(bool)) {
        bool v1 = boost::any_cast<bool>(value);
        bool v2 = psp->get<bool>(name);
        return (v1 == v2);
    } else if (psp->typeOf(name) == typeid(float)) {
        float v1 = boost::any_cast<float>(value);
        float v2 = psp->get<float>(name);
        return (v1 == v2);
    }
    return false;
}

/** private method to check the internal message cache to be sure that the
  * message we're looking for hasn't already been received.
  */
PropertySet::Ptr EventReceiver::checkMessageCache(const std::string& name, boost::any value) {
    list<PropertySet::Ptr>::iterator it;
    for (it = _messageCache.begin(); it != _messageCache.end(); it++) {
        if (matches(*it, name, value)) {
                return (*it);
        }
    }
    return PropertySet::Ptr();
}

/** private method to handle reading a local message from a socket
  */
PropertySet::Ptr EventReceiver::processStandaloneMessage(int remoteSocket) {
        int messageLength;

        int len = read(remoteSocket, &messageLength, 4);
        char *buf = (char *)malloc(messageLength+1);

        len = read(remoteSocket, buf, messageLength);
        buf[len] = 0;

        std::string text(buf);
        free(buf);

        PropertySet::Ptr retVal = unmarshall(text);
        return retVal;
}

/** private method unmarshall the DataProperty from the TextMessage
  */
PropertySet::Ptr EventReceiver::processTextMessage(const cms::TextMessage* textMessage) {
    if (textMessage == NULL)
        return PropertySet::Ptr();

    std::string text = textMessage->getText();
    return unmarshall(text);
}

/** private method unmarshall the DataProperty from a text string
  */
PropertySet::Ptr EventReceiver::unmarshall(const std::string& text) {
    std::vector<string> tuples;

    // split the text into tuples
    splitString(text, "~~", tuples);
std::cout << text << std::endl;

    PropertySet::Ptr psp(new PropertySet);

    unsigned int i;
    for (i = 0; i < tuples.size(); i++) {
        std::string line = tuples.at(i);
        std::vector<string> vec;
        splitString(line, "||", vec);

        std::string type = vec.at(0);
        std::string key = vec.at(1);
        std::string val = vec.at(2);

        std::istringstream iss(val);
        boost::any value;

        if (type == "int") {
            int int_value;
            iss >> int_value;
            value = boost::any(int_value);
            psp->add(key, int_value);
        } else if (type == "bool") {
            bool bool_value;
            iss >> bool_value;
            value = boost::any(bool_value);
            psp->add(key, bool_value);
        } else if (type == "long long") {
            long long longlong_value;
            iss >> longlong_value;
            value = boost::any(longlong_value);
            psp->add(key, longlong_value);
        } else if (type == "long") {
            long long_value;
            iss >> long_value;
            value = boost::any(long_value);
            psp->add(key, long_value);
        } else if (type == "float") {
            float float_value;
            iss >> float_value;
            value = boost::any(float_value);
            psp->add(key, float_value);
        } else if (type == "double") {
            double double_value;
            iss >> double_value;
            value = boost::any(double_value);
            psp->add(key, double_value);
        } else if (type == "string") {
            std::string string_value;
            iss >> string_value;
            psp->add(key, string_value);
        }
        // type == nodelist can be ignored
    }
        
    return psp;
}

/** private method to split a string along it's delimitors and return the
  * results in a vector
  */
void EventReceiver::splitString(std::string str, std::string delim, 
                                std::vector<std::string>&results) {
    std::string::size_type cutAt;
    std::string::size_type delim_len = delim.length();
    while( (cutAt = str.find(delim)) != str.npos ) {
        if(cutAt > 0) {
            results.push_back(str.substr(0,cutAt));
        }
        str = str.substr(cutAt+delim_len);
    }
    if(str.length() > 0) {
        results.push_back(str);
    }
}

/** \brief returns the topic for this EventReceiver
  */
std::string EventReceiver::getTopicName() {
    return _topic;
}

/** \brief destructor method
  */
EventReceiver::~EventReceiver() {

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
        if( _consumer != NULL )
            delete _consumer;
    } catch ( cms::CMSException& e ) {
        e.printStackTrace();
    }
    _consumer = NULL;

    // Close open resources.
    try {
        if( _session != NULL )
            _session->close();
    } catch ( cms::CMSException& e ) {
        e.printStackTrace();
    }
    try {
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
