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
    static const std::string ORIGINATORID;
    static const std::string LOCALID;
    static const std::string PROCESSID;
    static const std::string IPID;

    StatusEvent();
    virtual ~StatusEvent();

    StatusEvent(cms::TextMessage *msg);
    StatusEvent(const std::string& runid, int64_t originator, const PropertySet& ps);
    StatusEvent(const std::string& runid, int64_t originator, const PropertySet::Ptr psp);

    virtual void populateHeader(cms::TextMessage *msg) const;


    short getProcessId();
    short getLocalId();
    int getIPId();
    int64_t getOriginatorId();
    void setOriginatorId(int64_t id);

private:
    void _init();
    void _constructor(const std::string& runid, int64_t originator, const PropertySet& ps);

};
}
}
}


#endif /*end LSST_CTRL_EVENTS_STATUSEVENT_H*/
