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

#ifndef LSST_CTRL_EVENTS_COMMANDEVENT_H
#define LSST_CTRL_EVENTS_COMMANDEVENT_H

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
    static const std::string ORIGINATORID;
    static const std::string ORIG_LOCALID;
    static const std::string ORIG_PROCESSID;
    static const std::string ORIG_IPID;

    static const std::string DESTINATIONID;
    static const std::string DEST_LOCALID;
    static const std::string DEST_PROCESSID;
    static const std::string DEST_IPID;

    CommandEvent();
    CommandEvent(const std::string& runid, long originatorId, long destinationId, const PropertySet::Ptr psp);
    CommandEvent(cms::TextMessage *msg, const PropertySet::Ptr psp);
    virtual void populateHeader(cms::TextMessage *msg) const;

    ~CommandEvent();

    long getOriginatorId();
    short getOriginatorLocalId();
    short getOriginatorProcessId();
    int getOriginatorIPId();

    long getDestinationId();
    short getDestinationLocalId();
    short getDestinationProcessId();
    int getDestinationIPId();

private:
    virtual void setKeywords(PropertySet::Ptr psp) const;
    void _init();


    long _originatorId;      // long value of pid localid and hostid combined.
    short _orig_localId;    // created by EventSystem
    short _orig_processId;        // process id 
    int _orig_IPId;     // hex value of ip addr

    long _destinationId;     // long value of pid localid and hostid combined.
    short _dest_localId;    // created by EventSystem
    short _dest_processId;        // process id 
    int _dest_IPId;     // hex value of ip addr

};
}
}
}


#endif /*end LSST_CTRL_EVENTS_COMMANDEVENT_H*/
