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
 * @file Event.h
 *
 * @ingroup ctrl/events
 *
 * @brief defines the Event class
 *
 */

#ifndef LSST_CTRL_EVENTS_EVENT_H
#define LSST_CTRL_EVENTS_EVENT_H

#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/Message.h>

#include <stdlib.h>
#include <iostream>
#include <set>

#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"
#include "boost/property_tree/ptree.hpp"

using lsst::daf::base::PropertySet;

using namespace std;

namespace lsst {
namespace ctrl {
namespace events { 

/**
 * @class Event
 * @brief Representation of an LSST Event
 */

class Event
{
public:

    static const std::string TYPE;
    static const std::string EVENTTIME;
    static const std::string RUNID;
    static const std::string STATUS;
    static const std::string TOPIC;
    static const std::string PUBTIME;
    static const std::string UNINITIALIZED;

    /**
     * @brief Constructor for Event with no properties
     */
    Event();

    /**
     * @brief Constructor for Event
     * @param[in] properties the PropertySet to use to populate the event
     */
    Event(PropertySet const& properties);

    /**
     * @brief Constructor for Event
     * @param[in] properties the PropertySet to use to populate the event
     * @param[in] filterable PropertySet of types to be added to the header 
     *          so they can be filtered.
     */
    Event(PropertySet const& properties, PropertySet const& filterable);

    /**
     * @brief Constructor for Event
     * @param[in] runid A "run id" to place in the header of this event
     * @param[in] properties the PropertySet::Ptr to use to populate the event
     */
    Event(std::string const& runid, PropertySet::Ptr const properties);

    /**
     * @brief Constructor for Event
     * @param[in] runid A "run id" to place in the header of this event
     * @param[in] properties the PropertySet to use to populate the event
     */
    Event(std::string const& runid, PropertySet const& properties);

    /**
     * @brief Constructor for Event
     * @param[in] runid A "run id" to place in the header of this event
     * @param[in] properties the PropertySet to use to populate the event
     * @param[in] filterable PropertySet of types to be added to the header 
     *          so they can be filtered.
     */
    Event(std::string const& runid, PropertySet const& properties, PropertySet const& filterable);
    /**
     * @brief Constructor for Event
     * @param[in] msg A cms::TextMessage to convert into an Event object
     */
    Event(cms::TextMessage *msg);

    /**
     * @brief destructor
     */
    virtual ~Event();

    /**
     * @brief retrieve the PropertySet for this Event
     * @return PropertySet::Ptr to this PropertySet
     */
    PropertySet::Ptr getPropertySet() const;

    /**
     * @brief get the publication date of this Event, in ASCII
     */
    std::string getPubDate();

    /**
     * @brief get the publication time of this Event
     * @return time in nanoseconds
     */
    long long getPubTime();

    /**
     * @brief set the publication time of this Event
     */
    void setPubTime(long long t);

    /**
     * @brief get the event creation time
     * @return time in nanoseconds
     */
    long long getEventTime();

    /**
     * @brief set the event creation time
     */
    void setEventTime(long long nsecs);

    /**
     * @brief update the Event time to the current time
     */
    void updateEventTime();

    /**
     * @brief get the Event Date
     * @return a formatted date string representing the Event creation time
     */
    std::string getEventDate();

    /**
     * @brief get the RunId for this Event
     * @return string representation of the run id
     */
    std::string getRunId();

    /**
     * @brief get the Event type
     * @return string representation of the Event type
     */
    std::string getType();

    /**
     * @brief get the Event status
     * @return string representation of the Event status
     */
    std::string getStatus();

    /**
     * @brief set the Event status
     * @param[in] status string representation of the Event status
     */
    void setStatus(std::string status);

    /**
     * @brief set the Event topic
     * @param[in] topic string representation of the Event topic
     */
    void setTopic(std::string topic);

    /**
     * @brief get the Event topic
     * @return string representation of the Event topic
     */
    std::string getTopic();

    /**
     * @brief return all filterable property names
     * @return a vector of filterable property names
     */
    vector<std::string> getFilterablePropertyNames();

    /**
     * @brief return all custom property names
     * @return a vector of custom property names
     */
    vector<std::string> getCustomPropertyNames();

    /**
     * @brief return all custom property set
     * @return a PropertySet::Ptr of custom properties
     */
    PropertySet::Ptr getCustomPropertySet() const;

    /**
     * @brief populate a cms::TextMessage header with properties
     * @param[in] msg a cms::TextMessage
     */
    virtual void populateHeader(cms::TextMessage* msg) const;

    /**
     * @brief marshall values in this event into a cms::TextMessage
     */
    void marshall(cms::TextMessage *msg);

protected:
    PropertySet::Ptr _psp;
    PropertySet::Ptr _filterable;
    set<std::string> _keywords;
    void _init();
    void _constructor(std::string const& runid, PropertySet const& properties, PropertySet const& filterable);

    template<typename T>void add(std::string const& name, std::string const& tag, PropertySet const& properties, boost::property_tree::ptree& child);


private:
    std::string marshall(PropertySet const& properties);
    PropertySet::Ptr processTextMessage(cms::TextMessage *textMessage);
    PropertySet::Ptr unmarshall(std::string const& text);
    PropertySet::Ptr parsePropertySet(boost::property_tree::ptree child);
    bool addDataItem(std::string const& typeInfo, boost::property_tree::ptree& item, std::string const& key, PropertySet& properties);
};

}
}
}


#endif /*end LSST_CTRL_EVENTS_EVENT_H*/
