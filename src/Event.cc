// -*- lsst-c++ -*-
/** \file Event.cc
  *
  * \brief Object to transmit information through the event framework.
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
#include "lsst/ctrl/events/EventTypes.h"
#include "lsst/daf/base/DateTime.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/exceptions.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <time.h>
#include "lsst/ctrl/events/EventLibrary.h"

#include <activemq/core/ActiveMQConnectionFactory.h>

namespace pexExceptions = lsst::pex::exceptions;
namespace pexLogging = lsst::pex::logging;
namespace dafBase = lsst::daf::base;


using namespace std;
using std::numeric_limits;

namespace lsst {
namespace ctrl {
namespace events {

/** \brief Creates Event which contains a PropertySet
  *
  * \throw throws NotFoundException if expected keywords are missing a property set
  */

// NOTE:  While it would be nice to implement this as a wrapper for a TextMessage,
//        the JMS/ActiveMQ API doesn't have straight constructors Message objects.
//        They are all created through a JMS/ActiveMQ Session object.  This
//        makes implementation of some methods (like getFilterablePropertyNames)
//        more than just a straight call to a JMS method, since there is no
//        underlying Message wrapped.

const std::string Event::TYPE = "TYPE";
const std::string Event::EVENTTIME = "EVENTTIME";
const std::string Event::HOSTID = "HOSTID";
const std::string Event::RUNID = "RUNID";
const std::string Event::STATUS = "STATUS";
const std::string Event::TOPIC = "TOPIC";
const std::string Event::PUBTIME = "PUBTIME";

Event::Event() {
    _init();
}

void Event::_init() {
    _keywords.insert(TYPE);
    _keywords.insert(EVENTTIME);
    _keywords.insert(HOSTID);
    _keywords.insert(RUNID);
    _keywords.insert(STATUS);
    _keywords.insert(TOPIC);
    _keywords.insert(PUBTIME);
}

Event::Event(cms::TextMessage *msg) {
    _init();

    _psp = processTextMessage(msg);

    _psp->set(TYPE, msg->getStringProperty(TYPE));
    _psp->set(HOSTID, msg->getStringProperty(HOSTID));
    _psp->set(RUNID, msg->getStringProperty(RUNID));
    _psp->set(STATUS, msg->getStringProperty(STATUS));
    _psp->set(TOPIC, msg->getStringProperty(TOPIC));
    _psp->set(EVENTTIME, msg->getLongProperty(EVENTTIME));
    _psp->set(PUBTIME, msg->getLongProperty(PUBTIME));
/*
*/
}

vector<std::string> Event::getFilterablePropertyNames() {
    vector<std::string> _names;
    set<std::string>::iterator keyIterator;
    for (keyIterator = _keywords.begin(); keyIterator != _keywords.end(); keyIterator++) {
        _names.push_back(*keyIterator);
    }
    return _names;
}

vector<std::string> Event::getCustomPropertyNames() {
    vector<std::string> names = _psp->names();

    vector<std::string>::iterator nameIterator;
    set<std::string>::iterator keyIterator;

    for (nameIterator = names.begin(); nameIterator != names.end();) {
            keyIterator = _keywords.find(*nameIterator);
            if (keyIterator == _keywords.end())
                nameIterator++;
            else
                names.erase(nameIterator);
    }
    return names;
}

Event::Event( const std::string& runId, const PropertySet::Ptr psp) {
    _constructor(runId, *psp);
}

Event::Event( const std::string& runId, const PropertySet& ps) {
    _constructor(runId, ps);
}

void Event::_constructor( const std::string& runId, const PropertySet& ps) {
    char hostname[HOST_NAME_MAX];
    //time_t rawtime;

    _init();
    
    _psp = ps.deepCopy();


    // do NOT alter the property set we were given. Make a copy of it,
    // and modify that one.

    /*
    std::string foo = psp->toString();
    std::cout << "foo = " << foo << std::endl;
    */

    if (!_psp->exists(STATUS)) {
        _psp->set(STATUS, "unknown");
    }
    
    if (!_psp->exists(EVENTTIME)) {
        updateEventTime();
    }
   

    if (!_psp->exists(HOSTID)) {
        std::string name;
        gethostname(hostname, HOST_NAME_MAX);
        name = hostname;
        _psp->set(HOSTID, name);
    }

    // _runId is filled in here and is ignored in the passed PropertySet
    _psp->set(RUNID, runId);

    // _type is filled in here and is ignored in the passed PropertySet
    _psp->set(TYPE, EventTypes::EVENT);

    // _topic is filled in on publish and is ignored in the passed PropertySet
    _psp->set(TOPIC, "uninitialized");

    // _pubTime is filled in on publish and is ignored in the passed PropertySet
    _psp->set(PUBTIME, (long long)0);
}

void Event::populateHeader(cms::TextMessage* msg) {
    msg->setStringProperty(TYPE, _psp->get<std::string>(TYPE));
    msg->setLongProperty(EVENTTIME, _psp->get<long long>(EVENTTIME));
    msg->setStringProperty(HOSTID, _psp->get<std::string>(HOSTID));
    msg->setStringProperty(RUNID, _psp->get<std::string>(RUNID));
    msg->setStringProperty(STATUS, _psp->get<std::string>(STATUS));
    msg->setStringProperty(TOPIC, _psp->get<std::string>(TOPIC));
    msg->setLongProperty(PUBTIME, _psp->get<long long>(PUBTIME));
}

long long Event::getEventTime() {
    return _psp->get<long long>(EVENTTIME);
}

void Event::setEventTime(long long nsecs) {
    _psp->set(EVENTTIME,  nsecs);
}

void Event::updateEventTime() {
    _psp->set(EVENTTIME,  dafBase::DateTime::now().nsecs());
}

/** \brief Get the creation date of this event
  * \return A formatted date string representing the event creation time
  */
std::string Event::getEventDate() {
    dafBase::DateTime dateTime(_psp->get<long long>(EVENTTIME));
    
    struct tm eventTime = dateTime.gmtime();
    return asctime(&eventTime);
}


PropertySet::Ptr Event::getCustomPropertySet() const {
    PropertySet::Ptr psp = _psp->deepCopy();

    set<std::string>::iterator keyIterator;
    for (keyIterator = _keywords.begin(); keyIterator != _keywords.end(); keyIterator++) 
        psp->remove(*keyIterator);
    return psp;
}

PropertySet::Ptr Event::getPropertySet() const {
    PropertySet::Ptr psp = _psp->deepCopy();

    return psp;
}

void Event::setPubTime(long long t) {
    _psp->set(PUBTIME, t);
}

long long Event::getPubTime() {
    return _psp->get<long long>(PUBTIME);
}

/** \brief Get the publication date of this event
  * \return A formatted date string represeting the publication time
  */
std::string Event::getPubDate() {
    long long _pubTime = _psp->get<long long>(PUBTIME);
    if (_pubTime == 0)
        return std::string();

    dafBase::DateTime dateTime(_pubTime);
    
    struct tm pubTime = dateTime.gmtime();
    return asctime(&pubTime);
}


std::string Event::getHostId() {
    return _psp->get<std::string>(HOSTID);
}

std::string Event::getRunId() {
    return _psp->get<std::string>(RUNID);
}

std::string Event::getType() {
    return _psp->get<std::string>(TYPE);
}

std::string Event::getStatus() {
    return _psp->get<std::string>(STATUS);
}

void  Event::setStatus(std::string status) {
    return _psp->set(STATUS, status);
}

void Event::setTopic(std::string topic) {
    _psp->set(TOPIC, topic);
}

std::string Event::getTopic() {
    return _psp->get<std::string>(TOPIC);
}

void Event::marshall(cms::TextMessage *msg) {
    PropertySet::Ptr psp;

    populateHeader(msg);
    psp = getCustomPropertySet();
    std::string payload = marshall(*psp);
    msg->setText(payload);
}


std::string Event::marshall(const PropertySet& ps) {
    std::vector<std::string> v = ps.paramNames(false);

    // TODO: optimize this to get use getArray only when necessary
    std::ostringstream payload;
    unsigned int i;
    payload << "nodelist||nodelist||" << (v.size()) << "~~";
    for (i = 0; i < v.size(); i++) {
        std::string name = v[i];
        if (ps.typeOf(name) == typeid(bool)) {
            std::vector<bool> vec  = ps.getArray<bool>(name);
            std::vector<bool>::iterator iter;
            for (iter = vec.begin(); iter != vec.end(); iter++) {
                payload << "bool||"<< name << "||" << *iter << "~~";
            }
        } else if (ps.typeOf(name) == typeid(long)) {
            std::vector<long> vec  = ps.getArray<long>(name);
            std::vector<long>::iterator iter;
            for (iter = vec.begin(); iter != vec.end(); iter++) {
                payload << "long||" << name << "||"<< *iter << "~~";
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
        } else {
            std::cout << "Couldn't marshall "<< name << std::endl;
        }
    }
    return payload.str();
}

/** private method unmarshall the DataProperty from the TextMessage
  */
PropertySet::Ptr Event::processTextMessage(cms::TextMessage* textMessage) {
    if (textMessage == NULL)
        return PropertySet::Ptr();

    std::string text = textMessage->getText();

    return unmarshall(text);
}

/** private method unmarshall the DataProperty from a text string
  */
PropertySet::Ptr Event::unmarshall(const std::string& text) {
    std::vector<string> tuples;

    // split the text into tuples
    splitString(text, "~~", tuples);


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
            psp->add(key, val);
        } else if (type == "datetime") {
            long long longlong_value;
            iss >> longlong_value;
            psp->add(key, lsst::daf::base::DateTime(longlong_value, lsst::daf::base::DateTime::UTC));
        }
        // type == nodelist can be ignored
    }
        
    return psp;
}

/** private method to split a string along it's delimitors and return the
  * results in a vector
  */
void Event::splitString(std::string str, std::string delim, 
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

/** \brief destructor
  */
Event::~Event() {
}

}
}
}

