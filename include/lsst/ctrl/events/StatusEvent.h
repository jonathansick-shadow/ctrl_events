// -*- lsst-c++ -*-
/** \file LogEvent.h
  *
  * \ingroup events
  *
  * \brief defines the LogEvent class
  *
  * \author Stephen Pietrowicz, NCSA
  *
  */

#ifndef LSST_CTRL_EVENTS_STATUSEVENT_H
#define LSST_CTRL_EVENTS_STATUSEVENT_H

#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/Message.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/ctrl/events/Event.h"
#include "lsst/pex/logging/LogRecord.h"
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

class StatusEvent : public Event
{
public:
    StatusEvent();
    
    StatusEvent(cms::TextMessage *msg, const PropertySet::Ptr psp);
    StatusEvent(const std::string& runid, const PropertySet::Ptr psp);
    virtual void populateHeader(cms::TextMessage *msg) const;

    ~StatusEvent();

    unsigned short getProcessId();
    unsigned short getLocalId();
    unsigned int getIPId();
    unsigned long getOriginatorId();

private:
    virtual void setKeywords(PropertySet::Ptr psp) const;
    void _init();

    unsigned short _processId;        // process id 
    unsigned short _localId;    // created by EventSystem
    unsigned int _IPId;     // hex value of ip addr
    unsigned long _originatorId; // long value of pid localid and hostid combined.

};
}
}
}


#endif /*end LSST_CTRL_EVENTS_STATUSEVENT_H*/
