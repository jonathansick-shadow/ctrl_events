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
 * @file CommandEvent.h
 *
 * @ingroup ctrl/events
 *
 * @brief defines the CommandEvent class
 *
 */

#ifndef LSST_CTRL_EVENTS_COMMANDEVENT_H
#define LSST_CTRL_EVENTS_COMMANDEVENT_H

#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/BytesMessage.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/base.h"
#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/LocationId.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"

using lsst::daf::base::PropertySet;

namespace lsst {
namespace ctrl {
namespace events { 

/**
 * @class CommandEvent
 * @brief Representation of an LSST CommandEvent
 */

class CommandEvent : public Event
{
public:

    static const std::string ORIG_HOSTNAME;
    static const std::string ORIG_PROCESSID;
    static const std::string ORIG_LOCALID;

    static const std::string DEST_HOSTNAME;
    static const std::string DEST_PROCESSID;
    static const std::string DEST_LOCALID;

    /** @brief Creates CommandEvent which contains a PropertySet
     *        consisting of an origination location ID and 
     *        a destination location ID, plus additional
     *        properties.
     */
    CommandEvent();

    /**
     * @brief Constructor for CommandEvent
     * @param originator originating location of this event
     * @param destination destination location for this event
     * @param psp PropertySet to pass in this event
     */
    CommandEvent(LocationId const& originator, LocationId const& destination, CONST_PTR(PropertySet)& psp);


    /**
     * @brief Constructor for CommandEvent
     * @param originator originating location of this event
     * @param destination destination location for this event
     * @param ps PropertySet to pass in this event
     */
    CommandEvent(LocationId const& originator, LocationId const& destination, PropertySet const& ps);

    /**
     * @brief Constructor for CommandEvent
     * @param originator originating location of this event
     * @param destination destination location for this event
     * @param ps PropertySet to pass in this event
     * @param filterable additional, broker-filterable, PropertySet parameters
     */
    CommandEvent(LocationId const& originator, LocationId const& destination, PropertySet const& ps, 
                    PropertySet const& filterable);

    /**
     * @brief Constructor for CommandEvent
     * @param runid name of the run which this event is used in
     * @param originator originating location of this event
     * @param destination destination location for this event
     * @param psp PropertySet to pass in this event
     */
    CommandEvent(std::string const& runid, LocationId const& originator, LocationId const& destination, 
                    CONST_PTR(PropertySet)& psp);

    /**
     * @brief Constructor for CommandEvent
     * @param runid name of the run which this event is used in
     * @param originator originating location of this event
     * @param destination destination location for this event
     * @param ps PropertySet to pass in this event
     */
    CommandEvent(std::string const& runid, LocationId const& originator, LocationId const& destination, 
                    PropertySet const& ps);

    /**
     * @brief Constructor for CommandEvent
     * @param runid name of the run which this event is used in
     * @param originator originating location of this event
     * @param destination destination location for this event
     * @param ps PropertySet to pass in this event
     * @param filterable additional, broker-filterable, PropertySet parameters
     */
    CommandEvent(std::string const& runid, LocationId const& originator, LocationId const& destination, 
                    PropertySet const& ps, PropertySet const& filterable);

    /**
     * @brief Constructor for CommandEvent
     * @param msg a cms::TextMessage to convert into a CommandEvent
     */
    CommandEvent(cms::TextMessage *msg);

    /** 
     * @brief destructor
     */
    virtual ~CommandEvent();

    /**
     * @brief retrieve an object containing the Originator LocationId
     */
    PTR(LocationId) getOriginator() const;

    /**
     * @brief retrieve an object containing the Desination LocationId
     */
    PTR(LocationId) getDestination() const;


private:
    void _constructor(LocationId const& originator, LocationId const& destination);
    virtual void populateHeader(cms::TextMessage *msg) const;

    void _init();

};

}}}

#endif /*end LSST_CTRL_EVENTS_COMMANDEVENT_H*/
