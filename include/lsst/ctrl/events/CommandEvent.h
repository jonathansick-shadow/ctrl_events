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
 
/** \file CommandEvent.h
  *
  * \ingroup events
  *
  * \brief defines the CommandEvent class
  *
  * \author Stephen Pietrowicz, NCSA
  *
  */

#ifndef LSST_CTRL_EVENTS_COMMANDEVENT_H
#define LSST_CTRL_EVENTS_COMMANDEVENT_H

#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/BytesMessage.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/LocationID.h"
#include "lsst/pex/policy.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"

using lsst::daf::base::PropertySet;

using namespace std;

namespace lsst {
namespace ctrl {
namespace events { 

/**
 * @brief Representation of an LSST Event
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

    CommandEvent();

    CommandEvent(const LocationID& originator, const LocationID& destination, const PropertySet::Ptr psp);
    CommandEvent(const LocationID& originator, const LocationID& destination, const PropertySet& ps);
    CommandEvent(const LocationID& originator, const LocationID& destination, const PropertySet& ps, const PropertySet& filterable);

    CommandEvent(const std::string& runid, const LocationID& originator, const LocationID& destination, const PropertySet::Ptr psp);
    CommandEvent(const std::string& runid, const LocationID& originator, const LocationID& destination, const PropertySet& ps);
    CommandEvent(const std::string& runid, const LocationID& originator, const LocationID& destination, const PropertySet& ps, const PropertySet& filterable);

    CommandEvent(cms::TextMessage *msg);

    virtual void populateHeader(cms::TextMessage *msg) const;

    virtual ~CommandEvent();

    LocationID *getOriginator();

    LocationID * getDestination();

protected:
    void _constructor(const LocationID& originator, const LocationID& destination);

private:
    void _init();

};
}
}
}


#endif /*end LSST_CTRL_EVENTS_COMMANDEVENT_H*/
