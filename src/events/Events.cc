// -*- lsst-c++ -*-
/** \file Events.cc
  *
  * \brief Objects to send and receive Events to the specified event bus
  *
  * Author: Stephen R. Pietrowicz, NCSA
  *
  */
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "lsst/events/Events.h"
#include "lsst/mwi/data/DataProperty.h"
#include "lsst/mwi/data/SupportFactory.h"
#include "lsst/mwi/exceptions.h"
#include "lsst/mwi/utils/Component.h"
#include "lsst/mwi/policy/Policy.h"
#include "lsst/mwi/logging/LogRecord.h"
#include <sys/socket.h>
#include <sys/un.h>

#define TUPLECOUNT_PROPERTY "LSSTEVENTTUPLES"

using namespace lsst::mwi::data;
using namespace lsst::mwi::exceptions;
using namespace boost;

using namespace activemq::core;
using namespace activemq::util;
using namespace activemq::concurrent;
using namespace cms;
using namespace std;

namespace lsst {
namespace events {

EventTransmitter::EventTransmitter( const Policy& policy) {
    if (!policy.exists("topicName")) {
        throw NotFound("topicName not found in policy");
    }

    _turnEventsOff = policy.getBool("turnEventsoff", false);
    _useLocalSockets = policy.getBool("useLocalSockets", false);
    if (_useLocalSockets == false) {
        if (!policy.exists("hostName")) {
            throw NotFound("hostname must be specified with 'useLocalSockets' is false");
        }
    }
    init(policy.getString("hostName", ""), policy.getString("topicName"));
}

EventTransmitter::EventTransmitter( const std::string& hostName,
                                    const std::string& topicName) {
    _turnEventsOff = false;
    _useLocalSockets = false;
    init(hostName, topicName);
}

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
            throw SocketFailed();
        }
        remote.sun_family = AF_UNIX;
        std::string unix_socket = "/tmp/"+_topic;
        strcpy(remote.sun_path, unix_socket.c_str());
        len = strlen(remote.sun_path)+sizeof(remote.sun_family)+1;

        if (connect(_sock, (struct sockaddr *)&remote, len) == -1) {
            throw CouldNotConnect();
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

        ActiveMQConnectionFactory* connectionFactory =
            new ActiveMQConnectionFactory( brokerUri );

        _connection = connectionFactory->createConnection();
        _connection->start();

        delete connectionFactory;

        _session = _connection->createSession( Session::AUTO_ACKNOWLEDGE );

        // Create the destination topic
        _destination = _session->createTopic( topicName );

        // Create a MessageProducer from the Session to the Topic or Queue
        _producer = _session->createProducer( _destination );
        _producer->setDeliveryMode( DeliveryMode::NON_PERSISTENT );
    } catch ( CMSException& e ) {
        std::cout << e.getMessage() << std::endl;
    }
}
void EventTransmitter::publish(DataProperty dp) {
    publish("",dp);
}

void EventTransmitter::publish(const std::string& type, DataProperty dp) {
   DataProperty::PtrType node(new DataProperty(dp));
   publish(type,node);
}

void EventTransmitter::publish(DataProperty::PtrType dpt) {
    publish("",dpt);
}

void EventTransmitter::publish(const std::string& type, DataProperty::PtrType dpt) {
    int nTuples;

    if (_turnEventsOff == true)
        return;

    std::string messageText = marshall(dpt, &nTuples);
    publish(type, messageText, nTuples);
}

void EventTransmitter::publish(const std::string& type, const LogRecord& rec) {
    int nTuples;

    int nRecords = 0;
    const LogRecord::DataListT& data = rec.getData();
    std::string messageText;
    std::ostringstream strstream;

    if (_turnEventsOff == true)
        return;

    for (LogRecord::DataIteratorT i = data.begin(); i != data.end(); i++) {
        messageText = marshall((*i), &nTuples);
        strstream << messageText;
        nRecords++;
    }
    if (nRecords == 1) {
        publish(type, messageText, nTuples);
    } else {
        std::ostringstream header;
        header << "nodelist||nodelist||" << nRecords << "~~";
        header << strstream.str();
        publish(type, header.str(),nRecords);
    }
}

void EventTransmitter::publish(const std::string& type, const std::string& messageText, int nTuples) {

    // send the marshalled message to the local socket

    if (_useLocalSockets == true) {
        std::string str = messageText;
        int str_len = strlen(str.c_str());

        // number of tuples
        if (send(_sock, &nTuples, 4, 0) == -1) {
            perror("send");
        }

        // length of buffer
        if (send(_sock, &str_len, 4, 0) == -1) {
            perror("send");
        }

        // buffer
        if (send(_sock, str.c_str(), str_len, 0) == -1) {
            perror("send");
        }
        return;
    }

    // Sending the message to the ActiveMQ server
    TextMessage* message = _session->createTextMessage();
    message->setCMSType(type);

    message->setIntProperty(TUPLECOUNT_PROPERTY,nTuples);
    message->setText(messageText);
    _producer->send( message );

    delete message;
}

std::string EventTransmitter::marshall(const DataProperty::PtrType dpt, int *nTuples) {
    *nTuples = 0;
    std::ostringstream strstream;


    if (dpt->isNode() == true) {
            DataProperty::ContainerType::const_iterator pos;
            // root node of marshalled object
            const std::string& name(dpt->getName());

            DataProperty::iteratorRangeType list = dpt->getChildren();
            for (pos = list.first; pos != list.second; pos++) {
                if ((*pos)->isNode() == true) {
                    int nChildren;
                    std::string encoded = marshall((*pos), &nChildren);
                    strstream << encoded;
                } else {
                    strstream << encode(*pos);
                }
                *nTuples = (*nTuples) + 1;
            }
            std::ostringstream header;
            header << "node||"<< name << "||" << *nTuples << "~~";
            header << strstream.str();
            return header.str();
    } else {
        strstream << encode(dpt);
        *nTuples = (*nTuples) + 1;
    }
    return strstream.str();
}

std::string EventTransmitter::encode(const DataProperty::PtrType dpt) {
    std::ostringstream strstream;
    const std::string& child_name = any_cast<const std::string>(dpt->getName());
    boost::any val = dpt->getValue();

    // this may need to be augmented to handled long double
    if (val.type() == typeid(int)) {
        int value = any_cast<int>(val);
        strstream << "int||" << child_name << "||" << value << "~~";

    } else if (val.type() == typeid(long long)) {
        long long value = any_cast<long long>(val);
        strstream << "long long||" << child_name << "||" << value << "~~";

    } else if (val.type() == typeid(bool)) {
        bool value = any_cast<bool>(val);
        strstream << "bool||" << child_name << "||" << value << "~~";

    } else if (val.type() == typeid(double)) {
        double value = any_cast<double>(val);
        strstream << "double||" << child_name << "||" << std::setprecision(15) << value << "~~";

    } else if (val.type() == typeid(float)) {
        float value = any_cast<float>(val);
        strstream << "float||" << child_name << "||" << value << "~~";

    } else if (val.type() == typeid(std::string)) {
        const std::string& value = any_cast<const std::string>(val);
        strstream << "string||" << child_name << "||" << value << "~~";
    }
    return strstream.str();
}

std::string EventTransmitter::getTopicName() {
    return _topic;
}

EventTransmitter::~EventTransmitter() {

    if (_useLocalSockets == true) {
        close(_sock);
        return;
    }

    // Destroy resources.
    try {
        if( _destination != NULL )
            delete _destination;
    } catch ( CMSException& e ) {
        e.printStackTrace();
    }
    _destination = NULL;

    try {
        if( _producer != NULL )
            delete _producer;
    } catch ( CMSException& e ) {
        e.printStackTrace();
    }
    _producer = NULL;

    // Close open resources.
    try {
        if( _session != NULL )
            _session->close();
        if( _connection != NULL )
            _connection->close();
    } catch ( CMSException& e ) {
        e.printStackTrace();
    }

    try {
        if( _session != NULL )
            delete _session;
    } catch ( CMSException& e ) {
        e.printStackTrace();
    }
    _session = NULL;

    try {
        if( _connection != NULL )
            delete _connection;
    } catch ( CMSException& e ) {
        e.printStackTrace();
    }
    _connection = NULL;
}

EventReceiver::EventReceiver(const Policy& policy) {
    if (!policy.exists("topicName")) {
        throw NotFound("topicName not found in policy");
    }

    std::string topicName = policy.getString("topicName");
    _turnEventsOff = policy.getBool("turnEventsOff", false);
    _useLocalSockets = policy.getBool("useLocalSockets", false);
    if (_useLocalSockets == false) {
        if (!policy.exists("hostName")) {
            throw HostNotSpecified();
        }
    }


    init(policy.getString("hostName", "non"), topicName);
}

/** Create an EventReceiver listening for messages from
  * the message bus on hostName for topic topicName
  *
  * This constructor sets up the necessary connection to
  * the message bus.
  */
EventReceiver::EventReceiver(const std::string& hostName, const std::string& topicName) {
    _useLocalSockets = false;
    init(hostName, topicName);
}


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
            perror("socket");
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

        ActiveMQConnectionFactory* connectionFactory =
            new ActiveMQConnectionFactory( jmsURL );


        _connection = connectionFactory->createConnection();
        delete connectionFactory;
        _connection->start();

        _session = _connection->createSession( Session::AUTO_ACKNOWLEDGE );

        _destination = _session->createTopic( topicName );

        _consumer = _session->createConsumer( _destination );

    } catch (CMSException& e) {
        e.printStackTrace();
    }
}

DataProperty::PtrType EventReceiver::receive() {
    if (_messageCache.size() != 0) {
        DataProperty::PtrType dpt = _messageCache.front();
        if (dpt.get() != 0) {
            _messageCache.remove(dpt);
            return dpt;
        }
    }
    return _receive();
}

DataProperty::PtrType EventReceiver::_receive() {
    if (_useLocalSockets == false) {
        const TextMessage* textMessage =
            dynamic_cast<const TextMessage* >(_consumer->receive());
            return processTextMessage(textMessage);
    } else {
        return _receive(infiniteTimeout);
    }
}

DataProperty::PtrType EventReceiver::receive(long timeout) {
    if (_messageCache.size() != 0) {
        DataProperty::PtrType dpt = _messageCache.front();
        if (dpt.get() != 0) {
            _messageCache.remove(dpt);
            return dpt;
        }
    }
    return _receive(timeout);
}

DataProperty::PtrType EventReceiver::matchingReceive(const std::string& name, boost::any value, long timeout) {
    struct timeval tvStart;
    struct timezone tzStart;
    int sec, usec, total_usec, total_msec;
    int ret;

    DataProperty::PtrType dpt = checkMessageCache(name, value);
    if (dpt.get() != 0) {
        _messageCache.remove(dpt);
        return dpt;
    }

    /*
     * Receiving with timeout is slightly more complex when you're matching.  Messages could come
     * out of order, and if they do, you want to still wait for any time you might have remaining
     * to be sure the event you want doesn't come in during that time.
     */

    ret = gettimeofday(&tvStart, &tzStart);
    long currentTimeout = timeout;
    while (currentTimeout > 0) {
            dpt = _receive(currentTimeout);
            if (dpt.get() == 0)
                return DataProperty::PtrType();
            DataProperty::PtrType test = dpt->findUnique(name, 1);
            if (equal(test->getValue(), value) == true)
                return dpt;
            _messageCache.push_back(dpt);

            struct timeval tvCurrent;
            struct timezone tzCurrent;
            ret = gettimeofday(&tvCurrent, &tzCurrent);

            sec = tvCurrent.tv_sec-tvStart.tv_sec;
            usec = tvCurrent.tv_usec-tvStart.tv_usec;

            total_usec = sec*1000000+usec;
            total_msec = total_usec/1000;


            int timeLeft = timeout-total_msec;
            currentTimeout = timeLeft;
            if (currentTimeout <= 0)
                return DataProperty::PtrType();
    }
    return DataProperty::PtrType();
}

DataProperty::PtrType EventReceiver::_receive(long timeout) {
    if (_turnEventsOff == true)
        return DataProperty::PtrType();

    if (_useLocalSockets == false) {
        const TextMessage* textMessage =
            dynamic_cast<const TextMessage* >(_consumer->receive(timeout));
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
        if (select_val == 0)
            return DataProperty::PtrType();
        else if (select_val < 0) {
            perror("select");
            return DataProperty::PtrType();
        }

        if (FD_ISSET(_sock, &readfds)) {
            struct sockaddr_un remote;
            int len = sizeof(struct sockaddr_un);
            int remote_sock = accept(_sock, (struct sockaddr *)&remote, (socklen_t *)&len);
            if (remote_sock < 0)
                perror("accept");
            return processStandaloneMessage(remote_sock);
        }
        return DataProperty::PtrType();
    }
}

DataProperty::PtrType EventReceiver::matchingReceive(const std::string& name, const std::string& value) {
    return matchingReceive(name, boost::any(value));
}

DataProperty::PtrType EventReceiver::matchingReceive(const std::string& name, int value) {
    return matchingReceive(name, boost::any(value));
}

DataProperty::PtrType EventReceiver::matchingReceive(const std::string& name, float value) {
    return matchingReceive(name, boost::any(value));
}

DataProperty::PtrType EventReceiver::matchingReceive(const std::string& name, double value) {
    return matchingReceive(name, boost::any(value));
}

DataProperty::PtrType EventReceiver::matchingReceive(const std::string& name, boost::any value) {

    // check queue first
    DataProperty::PtrType dpt = checkMessageCache(name, value);
    if (dpt.get() != 0) {
        _messageCache.remove(dpt);
        return dpt;
    }
    while (true) { 
        dpt = _receive();
        // if match, return, otherwise stick it on the queue
        DataProperty::PtrType test = dpt->findUnique(name, 1);
        if (equal(test->getValue(), value) == true)
            return dpt;
        _messageCache.push_back(dpt);
    }
    return DataProperty::PtrType(); // to make the compiler happy
}

DataProperty::PtrType EventReceiver::matchingReceive(const std::string& name, const std::string& value, long timeout) {
    return matchingReceive(name, boost::any(value), timeout);
}

DataProperty::PtrType EventReceiver::matchingReceive(const std::string& name, int value, long timeout) {
    return matchingReceive(name, boost::any(value), timeout);
}

DataProperty::PtrType EventReceiver::matchingReceive(const std::string& name, float value, long timeout) {
    return matchingReceive(name, boost::any(value), timeout);
}

DataProperty::PtrType EventReceiver::matchingReceive(const std::string& name, double value, long timeout) {
    return matchingReceive(name, boost::any(value), timeout);
}

bool EventReceiver::equal(boost::any v1, boost::any v2) {
    if (v1.type() != v2.type())
        return false;

    if (v1.type() == typeid(int)) {
        int value1 = any_cast<int>(v1);
        int value2 = any_cast<int>(v2);
        if (value1 == value2)
            return true;
    } else if (v1.type() == typeid(long long)) {
        long long value1 = any_cast<long long>(v1);
        long long value2 = any_cast<long long>(v2);
        if (value1 == value2)
            return true;
    } else if (v1.type() == typeid(bool)) {
        bool value1 = any_cast<bool>(v1);
        bool value2 = any_cast<bool>(v2);
        if (value1 == value2)
            return true;
    } else if (v1.type() == typeid(double)) {
        double value1 = any_cast<double>(v1);
        double value2 = any_cast<double>(v2);
        if (value1 == value2)
            return true;
    } else if (v1.type() == typeid(float)) {
        float value1 = any_cast<float>(v1);
        float value2 = any_cast<float>(v2);
        if (value1 == value2)
            return true;
    } else if (v1.type() == typeid(std::string)) {
        std::string value1 = any_cast<std::string>(v1);
        std::string  value2 = any_cast<std::string>(v2);
        if (value1 == value2)
            return true;
    }

    return false;
}

DataProperty::PtrType EventReceiver::checkMessageCache(const std::string& element, boost::any value) {
    list<DataProperty::PtrType>::iterator it;
    for (it = _messageCache.begin(); it != _messageCache.end(); it++) {
        DataProperty::PtrType test = (*it)->findUnique(element, 1);
        if (equal(test->getValue(),value) == true) {
            _messageCache.remove(test);
            return (*it);
        }
    }    
    return DataProperty::PtrType();
}

DataProperty::PtrType EventReceiver::processStandaloneMessage(int remoteSocket) {
        int nTuples;
        int messageLength;

        int len = read(remoteSocket, &nTuples, 4);
        len = read(remoteSocket, &messageLength, 4);
        char *buf = (char *)malloc(messageLength+1);

        len = read(remoteSocket, buf, messageLength);
        buf[len] = 0;

        std::string text(buf);
        free(buf);

        DataProperty::PtrType retVal = unmarshall(nTuples, text);
        return retVal;
}

// unmarshall the DataProperty from the TextMessage
DataProperty::PtrType EventReceiver::processTextMessage(
                                        const TextMessage* textMessage) {
    if (textMessage == NULL)
        return DataProperty::PtrType();

    int nTuples = textMessage->getIntProperty(TUPLECOUNT_PROPERTY);
    std::string text = textMessage->getText();
    return unmarshall(nTuples, text);
}

#ifdef NOTDEF
DataProperty::PtrType EventReceiver::unmarshall(int nTuples, std::string text) {
    std::vector<string> vec;
    DataProperty::PtrType root;
    boost::any value;
    std::string type;
    std::string key;
    std::string val;


    splitString(text, "~~", vec);

    for (int i = 0; i < nTuples; i++) {
        std::string root_name;
        std::vector<string> vec2;
        std::string line = vec.at(i);
        splitString(line, "||", vec2);
        
        type = vec2.at(0);
        key = vec2.at(1);
        val = vec2.at(2);

        std::istringstream iss(val);
        if (type == "node") {
            root_name = key;
        } else if (type == "int") {
            int int_value;
            iss >> int_value;
            value = boost::any(int_value);
        } else if (type == "bool") {
            bool bool_value;
            iss >> bool_value;
            value = boost::any(bool_value);
        } else if (type == "float") {
            float float_value;
            iss >> float_value;
            value = boost::any(float_value);
        } else if (type == "double") {
            double double_value;
            iss >> double_value;
            value = boost::any(double_value);
        } else if (type == "string") {
            value = boost::any(val);
        }
            
        if (root.get() == 0) {
            root = SupportFactory::createPropertyNode(root_name);
        } else {
            DataProperty::PtrType node(new DataProperty(key, value));
            root->addProperty(node);
        }
    } 
    return root;
    
}
#endif

DataProperty::PtrType EventReceiver::unmarshall(int nTuples, std::string text) {
    int pos;
    std::vector<string> vec;
    std::vector<string> vec2;

    splitString(text, "~~", vec);

/*
    if (nTuples == 1) {
        DataProperty::PtrType node(unmarshall(nTuples, vec, pos));
        return node;
    }
*/
    
    
    std::string line = vec.at(0);
    splitString(line, "||", vec2);
        
    std::string type = vec2.at(0);
    std::string key = vec2.at(1);
    std::string val = vec2.at(2);

    DataProperty::PtrType root = SupportFactory::createPropertyNode(key);
    pos = 1;
    DataProperty::PtrType node(unmarshall(nTuples, root, vec, pos));
    
    return root;
}

DataProperty::PtrType EventReceiver::unmarshall(int nTuples, DataProperty::PtrType root, std::vector<string> vec, int& pos) {
    std::string type;
    std::string key;
    std::string val;

    for (int i = 0; i < nTuples; i++) {
        std::string root_name;
        std::vector<string> vec2;
        std::string line = vec.at(pos);
        splitString(line, "||", vec2);
        
        type = vec2.at(0);
        key = vec2.at(1);
        val = vec2.at(2);

        std::istringstream iss(val);
        if (type == "node") {
            int int_value;
            iss >> int_value;
            DataProperty::PtrType newroot = SupportFactory::createPropertyNode(key);
            pos++;
            DataProperty::PtrType node(unmarshall(int_value, newroot, vec, pos));
            root->addProperty(newroot);
        } else {
            boost::any value;
            if (type == "int") {
                int int_value;
                iss >> int_value;
                value = boost::any(int_value);
            } else if (type == "bool") {
                bool bool_value;
                iss >> bool_value;
                value = boost::any(bool_value);
            } else if (type == "long long") {
                long long longlong_value;
                iss >> longlong_value;
                value = boost::any(longlong_value);
            } else if (type == "float") {
                float float_value;
                iss >> float_value;
                value = boost::any(float_value);
            } else if (type == "double") {
                double double_value;
                iss >> double_value;
                value = boost::any(double_value);
            } else if (type == "string") {
                value = boost::any(val);
            }
            DataProperty::PtrType node(new DataProperty(key, value));
            root->addProperty(node);
            pos++;
        }
    } 
    return root;
    
}

void EventReceiver::splitString(std::string str, std::string delim, 
                                std::vector<std::string>&results) {
    unsigned int cutAt;
    int delim_len = delim.length();
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

std::string EventReceiver::getTopicName() {
    return _topic;
}

EventReceiver::~EventReceiver() {

    if (_useLocalSockets == true) {
        close(_sock);
        return;
    }

    // Destroy resources.
    try {
        if( _destination != NULL )
            delete _destination;
    } catch ( CMSException& e ) {
        e.printStackTrace();
    }
    _destination = NULL;

    try {
        if( _consumer != NULL )
            delete _consumer;
    } catch ( CMSException& e ) {
        e.printStackTrace();
    }
    _consumer = NULL;

    // Close open resources.
    try {
        if( _session != NULL )
            _session->close();
    } catch ( CMSException& e ) {
        e.printStackTrace();
    }
    try {
        if( _connection != NULL )
            _connection->close();
    } catch ( CMSException& e ) {
        e.printStackTrace();
    }

    try {
        if( _session != NULL )
            delete _session;
    } catch ( CMSException& e ) {
        e.printStackTrace();
    }
    _session = NULL;

    try {
        if( _connection != NULL )
            delete _connection;
    } catch ( CMSException& e ) {
        e.printStackTrace();
    }
    _connection = NULL;
}

}
}
