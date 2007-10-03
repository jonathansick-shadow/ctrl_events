// -*- lsst-c++ -*-
/** \file Events.h
  *
  * \brief Objects to send and receive Events to the specified event bus
  *
  * Author: Stephen R. Pietrowicz, NCSA
  *
  */
#include <iomanip>
#include <Python.h>
#include <sstream>
#include <stdexcept>

#include "lsst/events/Events.h"
#include "lsst/mwi/data/DataProperty.h"
#include "lsst/mwi/data/SupportFactory.h"
#include "lsst/mwi/utils/Component.h"
#include "lsst/mwi/policy/Policy.h"
#include <sys/socket.h>
#include <sys/un.h>

#define TUPLECOUNT_PROPERTY "LSSTEVENTTUPLES"

using namespace lsst::mwi::data;
using namespace boost;

using namespace activemq::core;
using namespace activemq::util;
using namespace activemq::concurrent;
using namespace cms;
using namespace std;

LSST_START_NAMESPACE(lsst)
LSST_START_NAMESPACE(events)

EventTransmitter::EventTransmitter( const Policy& policy) {
    if (!policy.exists("topicName")) {
        throw std::runtime_error("EventTransmitter policy requires \"topicName\"");
    }

    _turnEventsOff = policy.getBool("turnEventsoff", false);
    _useLocalSockets = policy.getBool("useLocalSockets", false);
    if (_useLocalSockets == false) {
        if (!policy.exists("hostName")) {
            throw std::runtime_error("EventTransmitter policy requires \"hostName\" if useLocalSockets is not set or is declared false");
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
            perror("socket");
        }
        remote.sun_family = AF_UNIX;
        std::string unix_socket = "/tmp/"+_topic;
        strcpy(remote.sun_path, unix_socket.c_str());
        len = strlen(remote.sun_path)+sizeof(remote.sun_family)+1;

        if (connect(_sock, (struct sockaddr *)&remote, len) == -1) {
            perror("connect");
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

void EventTransmitter::publish(const std::string& type, DataProperty::PtrType dpt) {
    int nTuples;

    if (_turnEventsOff == true)
        return;

    std::string messageText = marshall(dpt, &nTuples);

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

// marshall the DataProperty into a message we can send over the wire
// TODO:  When DataProperty becomes serializable, use that mechanism instead of
//        what we do here
std::string EventTransmitter::marshall(const DataProperty::PtrType dpt, int *nTuples) {
    *nTuples = 1;
    DataProperty::ContainerType::const_iterator pos;
    std::ostringstream strstream;
    std::string text;

    const std::string& name(dpt->getName());

    // root node of marshalled object
    strstream << "root||" << name << "||" << name << "~~";

    DataProperty::iteratorRangeType list = dpt->getChildren();
    for (pos = list.first; pos != list.second; pos++) {
        std::string tuple;

        const std::string& child_name = 
              any_cast<const std::string>((*pos)->getName());
        boost::any val = (*pos)->getValue();

        // this may need to be augmented to handled long double
        if (val.type() == typeid(int)) {
            int value = any_cast<int>(val);
            strstream << "int||" << child_name << "||" << value << "~~";

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
        *nTuples = (*nTuples) + 1;
    }
    return strstream.str();
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

EventReceiver::EventReceiver( Policy* policy) {
    _turnEventsOff = policy->getBool("turnEventsOff", false);
    _useLocalSockets = policy->getBool("useLocalSockets", false);
    if (_useLocalSockets == false) {
        if (!policy->exists("hostName")) {
            throw std::runtime_error("EventTransmitter policy requires \"hostName\" if useLocalSockets is not set or is declared false");
        }
    }

    if (!policy->exists("topicName")) {
        throw std::runtime_error("EventTransmitter policy requires \"topicName\"");
    }
    std::string topicName = policy->getString("topicName");

    init(policy->getString("hostName", "non"), topicName);
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
    if (_useLocalSockets == false) {
        const TextMessage* textMessage =
            dynamic_cast<const TextMessage* >(_consumer->receive());
            return processTextMessage(textMessage);
    } else {
        return receive(infiniteTimeout);
    }
}

DataProperty::PtrType EventReceiver::receive(long timeout) {
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

DataProperty::PtrType EventReceiver::unmarshall(int nTuples, std::string text) {
    std::vector<string> vec;
    DataProperty::PtrType root;
    boost::any value;
    std::string type;
    std::string key;
    std::string val;


    splitString(text, "~~", vec);

    for (int i = 0; i < nTuples; i++) {
        int int_value;
        float float_value;
        double double_value;
        std::string root_name;
        std::vector<string> vec2;
        std::string line = vec.at(i);
        splitString(line, "||", vec2);
        
        type = vec2.at(0);
        key = vec2.at(1);
        val = vec2.at(2);

        std::istringstream iss(val);
        if (type == "root") {
           root_name = key;
        } else if (type == "int") {
           iss >> int_value;
           value = boost::any(int_value);
        } else if (type == "float") {
           iss >> float_value;
           value = boost::any(float_value);
        } else if (type == "double") {
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


LSST_END_NAMESPACE(events)
LSST_END_NAMESPACE(lsst)
