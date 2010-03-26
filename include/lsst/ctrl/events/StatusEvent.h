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

#ifndef LSST_CTRL_EVENTS_LOGEVENT_H
#define LSST_CTRL_EVENTS_LOGEVENT_H

#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/BytesMessage.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/ctrl/events/Event.h"
#include "lsst/pex/logging/LogRecord.h"
#include "lsst/pex/policy.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/utils/Utils.h"
#include "lsst/daf/base/PropertySet.h"

using lsst::daf::base::PropertySet;

namespace pexPolicy = lsst::pex::policy;
namespace pexLogging = lsst::pex::logging;

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
    StatusEvent(const std::string& runid, const PropertySet::Ptr psp);

    ~StatusEvent();

    unsigned short getProcessId();
    unsigned short getLocalId();
    unsigned int getHostId();
    unsigned long getOriginator();

private:

    unsigned short pid;        // process id 
    unsigned short localid;    // created by EventSystem
    unsigned int hostid;     // hex value of ip addr
    unsigned long originator; // long value of pid localid and hostid combined.

};
}
}
}


#endif /*end LSST_CTRL_EVENTS_LOGEVENT_H*/
