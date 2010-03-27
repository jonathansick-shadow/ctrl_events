// -*- lsst-c++ -*-
/** \file CommandEvent.h
  *
  * \ingroup events
  *
  * \brief defines the CommandEvent class
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
    CommandEvent(const std::string& runid, unsigned long destination, const PropertySet::Ptr psp);

    ~CommandEvent();

    unsigned long getOriginator();
    unsigned short getOriginatorLocalId();
    unsigned short getOriginatorProcessId();
    unsigned int getOriginatorHostId();

    unsigned long getDestination();
    unsigned short getDestinationLocalId();
    unsigned short getDestinationProcessId();
    unsigned int getDestinationHostId();

private:

    unsigned long _originator;      // long value of pid localid and hostid combined.
    unsigned short _orig_localid;    // created by EventSystem
    unsigned short _orig_pid;        // process id 
    unsigned int _orig_hostid;     // hex value of ip addr

    unsigned long _destination;     // long value of pid localid and hostid combined.
    unsigned short _dest_localid;    // created by EventSystem
    unsigned short _dest_pid;        // process id 
    unsigned int _dest_hostid;     // hex value of ip addr

};
}
}
}


#endif /*end LSST_CTRL_EVENTS_LOGEVENT_H*/
