// -*- lsst-c++ -*-

/* 
 * LSST Data Management System
 * Copyright 2008-2014  AURA/LSST.
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
 * see <https://www.lsstcorp.org/LegalNotices/>.
 */

/** \file Event.cc
  *
  * \brief Object to transmit information through the event framework.
  *
  * \ingroup ctrl/events
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
#include <typeinfo>

#include "boost/scoped_array.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/foreach.hpp"

#include "lsst/ctrl/events/Host.h"
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
//    _keywords.insert(RUNID);
    _keywords.insert(STATUS);
    _keywords.insert(TOPIC);
    _keywords.insert(PUBTIME);
}

Event::Event(cms::TextMessage *msg) {
    // _init();

    vector<std::string>names = msg->getPropertyNames();
    unsigned int i;

    _psp = processTextMessage(msg);

    for (i = 0; i < names.size(); i++)
            _keywords.insert(names[i]);

    _psp->set(EVENTTIME, msg->getCMSTimestamp());

    for (i = 0; i < names.size(); i++) {
        std::string name = names[i];
        cms::Message::ValueType vType = msg->getPropertyValueType(names[i]);
        switch(vType) {
            case cms::Message::NULL_TYPE:
                _psp->set(name, NULL);
                break;
            case cms::Message::BOOLEAN_TYPE:
                _psp->set(name, msg->getBooleanProperty(name));
                break;
            case cms::Message::BYTE_TYPE:
            case cms::Message::CHAR_TYPE:
                _psp->set(name, msg->getByteProperty(name));
                break;
            case cms::Message::SHORT_TYPE:
                _psp->set(name, msg->getShortProperty(name));
                break;
            case cms::Message::INTEGER_TYPE:
                _psp->set(name, msg->getIntProperty(name));
                break;
            case cms::Message::LONG_TYPE:
                _psp->set(name, msg->getLongProperty(name));
                break;
            case cms::Message::DOUBLE_TYPE:
                _psp->set(name, msg->getDoubleProperty(name));
                break;
            case cms::Message::FLOAT_TYPE:
                _psp->set(name, msg->getFloatProperty(name));
                break;
            case cms::Message::STRING_TYPE:
                _psp->set(name, msg->getStringProperty(name));
                break;
            case cms::Message::BYTE_ARRAY_TYPE:
            case cms::Message::UNKNOWN_TYPE:
                break;
        }
    }
    
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

Event::Event(const PropertySet& ps) {
    const std::string empty;
    PropertySet::Ptr p (new PropertySet);
    _constructor(empty, ps, *p);
}

Event::Event(const PropertySet& ps, const PropertySet& filterable) {
    const std::string empty;
    _constructor(empty, ps, filterable);
}

Event::Event( const std::string& runId, const PropertySet::Ptr psp) {
    PropertySet::Ptr p (new PropertySet);
    _constructor(runId, *psp, *p);
}

Event::Event( const std::string& runId, const PropertySet& ps) {
    PropertySet::Ptr p (new PropertySet);
    _constructor(runId, ps, *p);
}

Event::Event( const std::string& runId, const PropertySet& ps, const PropertySet& filterable) {
    _constructor(runId, ps, filterable);
}

void Event::_constructor( const std::string& runId, const PropertySet& ps, const PropertySet& filterable) {
    long int host_len = sysconf(_SC_HOST_NAME_MAX);

    boost::scoped_array<char> hostname(new char[host_len]);

    _init();
    
    _psp = ps.deepCopy();


    // do NOT alter the property set we were given. Make a copy of it,
    // and modify that one.

    if (!_psp->exists(STATUS)) {
        _psp->set(STATUS, "unknown");
    }
    
    if (!_psp->exists(EVENTTIME)) {
        updateEventTime();
    }
   
    // _runId is filled in here and is ignored in the passed PropertySet
    if (!runId.empty()) {
        _keywords.insert(RUNID);
        _psp->set(RUNID, runId);
    }

    // _type is filled in here and is ignored in the passed PropertySet
    _psp->set(TYPE, EventTypes::EVENT);

    // _topic is filled in on publish and is ignored in the passed PropertySet
    _psp->set(TOPIC, Event::UNINITIALIZED);

    // _pubTime is filled in on publish and is ignored in the passed PropertySet
    _psp->set(PUBTIME, (long long)0);

    if (filterable.nameCount() > 0) {
        vector<std::string> names = filterable.names();
        unsigned int i;
        for (i = 0; i < names.size(); i++)
            _keywords.insert(names[i]);

        _psp->combine(filterable.PropertySet::deepCopy());
    }
}

void Event::populateHeader(cms::TextMessage* msg)  const {
    set<std::string>::iterator keyIterator;
    for (keyIterator = _keywords.begin(); keyIterator != _keywords.end(); keyIterator++) {
        std::string name = *keyIterator;
        const std::type_info &t = _psp->typeOf(name);
        if (t == typeid(bool))
            msg->setBooleanProperty(name, _psp->get<bool>(name));
        else if (t == typeid(short))
            msg->setShortProperty(name, _psp->get<short>(name));
        else if (t == typeid(int))
            msg->setIntProperty(name, _psp->get<int>(name));
        else if (t == typeid(long))
            msg->setLongProperty(name, _psp->get<long>(name));
        else if (t == typeid(long long))
            msg->setLongProperty(name, _psp->get<long long>(name));
        else if (t == typeid(double))
            msg->setDoubleProperty(name, _psp->get<double>(name));
        else if (t == typeid(float))
            msg->setFloatProperty(name, _psp->get<float>(name));
        else if (t == typeid(std::string))
            msg->setStringProperty(name, _psp->get<std::string>(name));
    }
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

std::string Event::getRunId() {
    if (_psp->exists(RUNID))
        return _psp->get<std::string>(RUNID);
    return std::string();
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

        boost::property_tree::ptree child = v.second;
        BOOST_FOREACH(boost::property_tree::ptree::value_type &v2, child) {
            std::string key2 = v2.first;

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

