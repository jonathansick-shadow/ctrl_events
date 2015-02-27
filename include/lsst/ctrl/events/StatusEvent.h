// -*- lsst-c++ -*-

/** \file StatusEvent.h
  *
  * \ingroup events
  *
  * \brief defines the StatusEvent class
  *
  */

#ifndef LSST_CTRL_EVENTS_STATUSEVENT_H
#define LSST_CTRL_EVENTS_STATUSEVENT_H

#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/Message.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/ctrl/events/LocationID.h"
#include "lsst/ctrl/events/Event.h"
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

class StatusEvent : public Event
{
public:
    static const std::string ORIG_HOSTNAME;
    static const std::string ORIG_PROCESSID;
    static const std::string ORIG_LOCALID;

    StatusEvent();
    virtual ~StatusEvent();

    StatusEvent(cms::TextMessage *msg);

    StatusEvent(const LocationID& originator, const PropertySet& ps);
    StatusEvent(const LocationID& originator, const PropertySet& ps, const PropertySet& filterable);

    StatusEvent(const std::string& runid, const LocationID& originator, const PropertySet& ps);
    StatusEvent(const std::string& runid, const LocationID& originator, const PropertySet& ps, const PropertySet& filterable);
    StatusEvent(const std::string& runid, const LocationID& originator, const PropertySet::Ptr psp);
    StatusEvent(const std::string& runid, const LocationID& originator, const PropertySet::Ptr psp, const PropertySet& filterable);

    virtual void populateHeader(cms::TextMessage *msg) const;


    LocationID *getOriginator();

private:
    void _init();
    void _constructor(const LocationID& originator);

};
}
}
}


#endif /*end LSST_CTRL_EVENTS_STATUSEVENT_H*/
