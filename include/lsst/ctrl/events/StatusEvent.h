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
 * @file StatusEvent.h
 *
 * @ingroup ctrl/events
 *
 * @brief defines the StatusEvent class
 *
 */

#ifndef LSST_CTRL_EVENTS_STATUSEVENT_H
#define LSST_CTRL_EVENTS_STATUSEVENT_H

#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/Message.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/base.h"
#include "lsst/ctrl/events/LocationId.h"
#include "lsst/ctrl/events/Event.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"

using lsst::daf::base::PropertySet;

namespace lsst {
namespace ctrl {
namespace events { 

/**
 * @class StatusEvent
 * @brief Representation of an LSST Event
 */

class StatusEvent : public Event
{
public:

    static std::string const ORIG_HOSTNAME;
    static std::string const ORIG_PROCESSID;
    static std::string const ORIG_LOCALID;

    /** 
     * @brief Constructor to create a StatusEvent
     */
    StatusEvent();
    virtual ~StatusEvent();

    /** 
     * @brief Constructor to convert a TextMessage into a StatusEvent
     */
    StatusEvent(cms::TextMessage *msg);

    /** 
     * @brief Constructor to create a StatusEvent
     * @param originator the LocationId of where this StatusEvent was created
     * @param ps a PropertySet
     */
    StatusEvent(LocationId const& originator, PropertySet const& ps);


    /** 
     * @brief Constructor to create a StatusEvent
     * @param originator the LocationId of where this StatusEvent was created
     * @param ps a PropertySet
     * @param filterable a PropertySet that will be added to Event headers so
     *        they can be filtered using selectors.
     */
    StatusEvent(LocationId const& originator, PropertySet const& ps, PropertySet const& filterable);

   /** 
    * @brief Constructor to create a StatusEvent
    * @param runid a string identify for this Event
    * @param originator the LocationId of where this StatusEvent was created
    * @param ps a PropertySet
    */
    StatusEvent(std::string const& runid, LocationId const& originator, PropertySet const& ps);

    /** 
     * @brief Constructor to create a StatusEvent
     * @param runid a string identify for this Event
     * @param originator the LocationId of where this StatusEvent was created
     * @param ps a PropertySet
     * @param filterable a PropertySet that will be added to Event headers so
     *        they can be filtered using selectors.
     */
    StatusEvent(std::string const& runid, LocationId const& originator, PropertySet const& ps, PropertySet const& filterable);

    /** 
     * \brief Constructor to create a StatusEvent
     * \param runid a string identify for this Event
     * \param originator the LocationId of where this StatusEvent was created
     * \param psp a PTR(PropertySet)
     */
    StatusEvent(std::string const& runid, LocationId const& originator, CONST_PTR(PropertySet) psp);


    /** 
     * @brief Constructor to create a StatusEvent
     * @param runid a string identify for this Event
     * @param originator the LocationId of where this StatusEvent was created
     * @param psp a PTR(PropertySet)
     * @param filterable a PropertySet that will be added to Event headers so
     *        they can be filtered using selectors.
     */
    StatusEvent(std::string const& runid, LocationId const& originator, CONST_PTR(PropertySet) psp, PropertySet const& filterable);


    /** 
     * @brief accessor to get originator information
     * @return a LocationId containing the Originator information
     */
    LocationId *getOriginator();

    /*  method used to take originator from the TextMessage to set in
     * the StatusEvent
     */
    virtual void populateHeader(cms::TextMessage *msg) const;

private:
    void _init();
    void _constructor(LocationId const& originator);


};
}
}
}


#endif /*end LSST_CTRL_EVENTS_STATUSEVENT_H*/
