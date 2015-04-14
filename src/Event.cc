// -*- lsst-c++ -*-

/* 
 * LSST Data Management System
 * Copyright 2008-2015  AURA/LSST.
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

/** 
 * @file Event.cc
 *
 * @ingroup ctrl/events
 *
 * @brief Object to transmit information through the event framework.
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
namespace dafBase = lsst::daf::base;


using namespace std;
using std::numeric_limits;

namespace lsst {
namespace ctrl {
namespace events {


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
    _keywords.insert(STATUS);
    _keywords.insert(TOPIC);
    _keywords.insert(PUBTIME);
}

Event::Event(cms::TextMessage *msg) {

    vector<std::string>names = msg->getPropertyNames();

    _psp = processTextMessage(msg);

    for (vector<std::string>::iterator name = names.begin(); name != names.end(); ++name) {
        _keywords.insert(*name);
    }

    _psp->set(EVENTTIME, msg->getCMSTimestamp());

    for (vector<std::string>::iterator n = names.begin(); n != names.end(); ++n) {
        std::string const& name = *n;
        cms::Message::ValueType vType = msg->getPropertyValueType(name);
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
            default:
                std::string msg("Data type represented by "+name+" is not permitted supported");
                throw LSST_EXCEPT(pexExceptions::RuntimeError, msg);
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

Event::Event(PropertySet const& ps) {
    const std::string empty;
    PropertySet p;
    _constructor(empty, ps, p);
}

Event::Event(PropertySet const& ps, PropertySet const& filterable) {
    const std::string empty;
    _constructor(empty, ps, filterable);
}

Event::Event(std::string const& runId, PropertySet::Ptr const psp) {
    PropertySet p;
    _constructor(runId, *psp, p);
}

Event::Event(std::string const& runId, PropertySet const& ps) {
    PropertySet p;
    _constructor(runId, ps, p);
}

Event::Event(std::string const& runId, PropertySet const& ps, PropertySet const& filterable) {
    _constructor(runId, ps, filterable);
}

void Event::_constructor(std::string const& runId, PropertySet const& ps, PropertySet const& filterable) {
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

        for (vector<std::string>::iterator name = names.begin(); name != names.end(); ++name) {
            _keywords.insert(*name);
        }

        _psp->combine(filterable.PropertySet::deepCopy());
    }
}

void Event::populateHeader(cms::TextMessage* msg)  const {
    set<std::string>::iterator keyIterator;
    for (keyIterator = _keywords.begin(); keyIterator != _keywords.end(); keyIterator++) {
        std::string const& name = *keyIterator;
        std::type_info const& t = _psp->typeOf(name);
        if (t == typeid(bool)) {
            msg->setBooleanProperty(name, _psp->get<bool>(name));
        } else if (t == typeid(short)) {
            msg->setShortProperty(name, _psp->get<short>(name));
        } else if (t == typeid(int)) {
            msg->setIntProperty(name, _psp->get<int>(name));
        } else if (t == typeid(long)) {
            msg->setLongProperty(name, _psp->get<long>(name));
        } else if (t == typeid(long long)) {
            msg->setLongProperty(name, _psp->get<long long>(name));
        } else if (t == typeid(double)) {
            msg->setDoubleProperty(name, _psp->get<double>(name));
        } else if (t == typeid(float)) {
            msg->setFloatProperty(name, _psp->get<float>(name));
        } else if (t == typeid(std::string)) {
            msg->setStringProperty(name, _psp->get<std::string>(name));
        } else {
            std::string msg("Data type represented in "+ name +" is not permitted in event header");
            throw LSST_EXCEPT(pexExceptions::RuntimeError, msg);
        }
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
    throw LSST_EXCEPT(pexExceptions::RuntimeError, std::string("property RUNID not found"));
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

template<typename T>void Event::add(std::string const& name, std::string const& tag, PropertySet const& ps, boost::property_tree::ptree& child) {

    std::vector<T> vec = ps.getArray<T>(name);

    typename std::vector<T>::iterator iter;
    for (iter = vec.begin(); iter != vec.end(); iter++) {
        boost::property_tree::ptree pt;
        pt.put(tag, *iter);
        child.put_child(name, pt);
    }
}

std::string Event::marshall(PropertySet const& ps) {
    std::vector<std::string> names = ps.paramNames(false);

    boost::property_tree::ptree child;
 
    for (vector<std::string>::iterator n = names.begin(); n != names.end(); ++n) {
        std::string name = *n;

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
            std::string msg("Couldn't marshall "+name);
            throw LSST_EXCEPT(pexExceptions::RuntimeError, msg);
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

/** private method to try to add data to a property set
 * \param typeInfo a string containing the name of the data type
 * \param item a node of a boost::property_tree::ptree containing data
 * \param key the name of the data
 * \param ps a PropertySet to store the name and data into.
 * \return true if data was added to the PropertySet
  */
bool Event::addDataItem(std::string const& typeInfo, boost::property_tree::ptree& item, std::string const&  key, PropertySet& ps) {
    if (typeInfo == "string") {
        std::string value = item.get_value<std::string>();
        ps.add(key, value);
    } else if (typeInfo == "bool") {
        bool value = item.get_value<bool>();
        ps.add(key, value);
    } else if (typeInfo == "long") {
        long value = item.get_value<long>();
        ps.add(key, value);
    } else if (typeInfo == "long long") {
        long long value = item.get_value<long long>();
        ps.add(key, value);
    } else if (typeInfo == "int") {
        int value = item.get_value<int>();
        ps.add(key, value);
    } else if (typeInfo == "float") {
        float value = item.get_value<float>();
        ps.add(key, value);
    } else if (typeInfo == "double") {
        double value = item.get_value<double>();
        ps.add(key, value);
    } else if (typeInfo == "datetime") {
        long long value = item.get_value<long long>();
        ps.add(key, lsst::daf::base::DateTime(value, lsst::daf::base::DateTime::UTC));
    } else {
        return false;
    }
    return true;
}

/** private method to iterate through the representation of a propertyset
  * within the boost::property_tree::ptree
  * \param child a boost::property_tree::ptree to iterate through
  * \return a PropertySet::Ptr containing the data that was stored in child.
  */
PropertySet::Ptr Event::parsePropertySet(boost::property_tree::ptree child) {
    PropertySet::Ptr psp(new PropertySet);
    
    BOOST_FOREACH(boost::property_tree::ptree::value_type const &v, child.get_child("")) {
        std::string label = v.first;
        BOOST_FOREACH(boost::property_tree::ptree::value_type &v2, child.get_child(label)) {
            const bool b = addDataItem(v2.first, v2.second, label, *psp);
            if (!b) {
                PropertySet::Ptr p2 = parsePropertySet(child.get_child(label));
                psp->add(label, p2);
                break;
            }
        }
    }
    return psp;
}

/** private method unmarshall the DataProperty from a text string
  * \param text a JSON text string
  * \return a PropertySet::Ptr containing the data that was stored in text
  */
PropertySet::Ptr Event::unmarshall(std::string const& text) {

    boost::property_tree::ptree pt;
    std::istringstream is (text);
    read_json(is, pt);

    PropertySet::Ptr psp(new PropertySet);

    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt) {
        std::string key = v.first;

        boost::property_tree::ptree child = v.second;
        BOOST_FOREACH(boost::property_tree::ptree::value_type &v2, child) {
            std::string key2 = v2.first;

            const bool b = addDataItem(key2, v2.second, key, *psp);
            if (!b) {
                std::string value = v2.second.get_value<std::string>();
                PropertySet::Ptr p = parsePropertySet(child);
                psp->add(key, p);
                break;
            }
        }
    }
        
    return psp;
}

Event::~Event() {
}

template void Event::add<bool>(std::string const& name, std::string const& tag, PropertySet const& ps, boost::property_tree::ptree& child);

template void Event::add<int>(std::string const& name, std::string const& tag, PropertySet const& ps, boost::property_tree::ptree& child);

template void Event::add<float>(std::string const& name, std::string const& tag, PropertySet const& ps, boost::property_tree::ptree& child);

template void Event::add<double>(std::string const& name, std::string const& tag, PropertySet const& ps, boost::property_tree::ptree& child);

template void Event::add<long>(std::string const& name, std::string const& tag,PropertySet const& ps, boost::property_tree::ptree& child);

template void Event::add<long long>(std::string const& name, std::string const& tag, PropertySet const& ps, boost::property_tree::ptree& child);

template void Event::add<std::string>(std::string const& name, std::string const& tag, PropertySet const& ps, boost::property_tree::ptree& child);

}}}
