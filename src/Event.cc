// -*- lsst-c++ -*-

/* 
 * LSST Data Management System
 * Copyright 2008, 2009, 2010 LSST Corporation.
 * 
 * This product includes software developed by the
 * LSST Project (http://www.lsst.org/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the LSST License Statement and 
 * the GNU General Public License along with this program.  If not, 
 * see <http://www.lsstcorp.org/LegalNotices/>.
 */
 
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
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <time.h>

#include "boost/scoped_array.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/foreach.hpp"

#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/EventTypes.h"
#include "lsst/daf/base/DateTime.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/exceptions.h"
#include "lsst/ctrl/events/EventLibrary.h"

#include "activemq/core/ActiveMQConnectionFactory.h"

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
  * \throw throws NotFoundError if expected keywords are missing a property set
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

const std::string Event::UNINITIALIZED = "uninitialized";

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
    long int host_len = sysconf(_SC_HOST_NAME_MAX);

    boost::scoped_array<char> hostname(new char[host_len]);
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
        gethostname(hostname.get(), host_len);
        name = hostname.get();
        _psp->set(HOSTID, name);
    }

    // _runId is filled in here and is ignored in the passed PropertySet
    _psp->set(RUNID, runId);

    // _type is filled in here and is ignored in the passed PropertySet
    _psp->set(TYPE, EventTypes::EVENT);

    // _topic is filled in on publish and is ignored in the passed PropertySet
    _psp->set(TOPIC, Event::UNINITIALIZED);

    // _pubTime is filled in on publish and is ignored in the passed PropertySet
    _psp->set(PUBTIME, (long long)0);
}

void Event::populateHeader(cms::TextMessage* msg)  const {
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
    _psp->set(EVENTTIME,  (long long)dafBase::DateTime::now().nsecs());
}

/** \brief Get the creation date of this event
  * \return A formatted date string representing the event creation time
  */
std::string Event::getEventDate() {
    long long eventTime = _psp->get<long long>(EVENTTIME);
    dafBase::DateTime dateTime(eventTime);
    
    struct tm gmTime = dateTime.gmtime();
    return asctime(&gmTime);
}


PropertySet::Ptr Event::getCustomPropertySet() const {
    PropertySet::Ptr psp = _psp->deepCopy();

    set<std::string>::iterator keyIterator;
    for (keyIterator = _keywords.begin(); keyIterator != _keywords.end(); keyIterator++) 
        psp->remove(*keyIterator);
    return psp;
}

PropertySet::Ptr Event::getPropertySet() const {
    if (_psp != 0) {
            PropertySet::Ptr psp = _psp->deepCopy();
            return psp;
    }
    PropertySet::Ptr psp(new PropertySet);
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

template<typename T>void Event::add(const std::string& name, const std::string& tag, const PropertySet& ps, boost::property_tree::ptree& child) {

    std::vector<T> vec = ps.getArray<T>(name);

    typename std::vector<T>::iterator iter;
    for (iter = vec.begin(); iter != vec.end(); iter++) {
        boost::property_tree::ptree pt;
        pt.put(tag, *iter);
        child.put_child(name, pt);
    }
}

std::string Event::marshall(const PropertySet& ps) {
    std::vector<std::string> v = ps.paramNames(false);

    boost::property_tree::ptree child;
    // TODO: optimize this to get use getArray only when necessary
    unsigned int i;
    for (i = 0; i < v.size(); i++) {
        std::string name = v[i];
        if (ps.typeOf(name) == typeid(bool)) {
            add<bool>(name, "bool", ps, child);
        } else if (ps.typeOf(name) == typeid(long)) {
            add<long>(name, "long", ps, child);
        } else if (ps.typeOf(name) == typeid(long long)) {
            add<long long>(name, "long long", ps, child);
        } else if (ps.typeOf(name) == typeid(int)) {
            add<int>(name, "int", ps, child);
        } else if (ps.typeOf(name) == typeid(float)) {
            add<float>(name, "float", ps, child);
        } else if (ps.typeOf(name) == typeid(double)) {
            add<double>(name, "double", ps, child);
        } else if (ps.typeOf(name) == typeid(std::string)) {
            add<std::string>(name, "string", ps, child);
        } else if (ps.typeOf(name) == typeid(lsst::daf::base::DateTime)) {
            std::vector<lsst::daf::base::DateTime> vec  = ps.getArray<lsst::daf::base::DateTime>(name);
            std::vector<lsst::daf::base::DateTime>::iterator iter;
            for (iter = vec.begin(); iter != vec.end(); iter++) {
                boost::property_tree::ptree pt;
                pt.put("datetime", (*iter).nsecs());
                child.put_child(name, pt);
            }
        } else {
            std::cout << "Couldn't marshall "<< name << std::endl;
        }
    }
    std::ostringstream payload;
    write_json(payload, child, false);
    std::cout << "payload is "<< payload.str() << std::endl;
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

    boost::property_tree::ptree pt;
    std::istringstream is (text);
    read_json(is, pt);

    PropertySet::Ptr psp(new PropertySet);

    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt) {
        std::string key = v.first;
        std::cout << "key: " << key << std::endl;
        boost::property_tree::ptree child = v.second;
        BOOST_FOREACH(boost::property_tree::ptree::value_type &v2, child) {
            std::string key2 = v2.first;
            std::cout << "key2: " << key2 << std::endl;
            if (key2 == "string") {
                std::string value = child.get<std::string>(key2);
                psp->add(key, value);
            } else if (key2 == "bool") {
                bool value = child.get<bool>(key2);
                psp->add(key, value);
            } else if (key2 == "long") {
                long value = child.get<long>(key2);
                psp->add(key, value);
            } else if (key2 == "long long") {
                long long value = child.get<long long>(key2);
                psp->add(key, value);
            } else if (key2 == "int") {
                int value = child.get<int>(key2);
                psp->add(key, value);
            } else if (key2 == "float") {
                float value = child.get<float>(key2);
                psp->add(key, value);
            } else if (key2 == "double") {
                double value = child.get<double>(key2);
                psp->add(key, value);
            } else if (key2 == "datetime") {
                long long value = child.get<long long>(key2);
                psp->add(key, lsst::daf::base::DateTime(value, lsst::daf::base::DateTime::UTC));
            }
        }
    }
        
    
    return psp;
}

/** private method to split a string along it's delimiters and return the
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

/** private method to split a tuple string along it's delimiters and return the
  * results in a vector; don't try and modify splitString to do this.
  */
void Event::splitTuple(std::string str, std::string delim, 
                                std::vector<std::string>&results) {
    std::string::size_type cutAt;
    std::string::size_type delim_len = delim.length();

    cutAt = str.find(delim);
    results.push_back(str.substr(0,cutAt));
    str = str.substr(cutAt+delim_len);

    cutAt = str.find(delim);
    results.push_back(str.substr(0,cutAt));
    str = str.substr(cutAt+delim_len);

    cutAt = str.find(delim);
    results.push_back(str.substr(0,cutAt));
}

/** \brief destructor
  */
Event::~Event() {
}

template void Event::add<bool>(const std::string& name, const std::string& tag, const PropertySet& ps, boost::property_tree::ptree& child);

template void Event::add<int>(const std::string& name, const std::string& tag, const PropertySet& ps, boost::property_tree::ptree& child);

template void Event::add<float>(const std::string& name, const std::string& tag, const PropertySet& ps, boost::property_tree::ptree& child);

template void Event::add<double>(const std::string& name, const std::string& tag, const PropertySet& ps, boost::property_tree::ptree& child);

template void Event::add<long>(const std::string& name, const std::string& tag, const PropertySet& ps, boost::property_tree::ptree& child);

template void Event::add<long long>(const std::string& name, const std::string& tag, const PropertySet& ps, boost::property_tree::ptree& child);

template void Event::add<std::string>(const std::string& name, const std::string& tag, const PropertySet& ps, boost::property_tree::ptree& child);
}
}
}

