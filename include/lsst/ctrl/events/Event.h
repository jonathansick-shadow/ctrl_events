// -*- lsst-c++ -*-
/** \file Event.h
  *
  * \ingroup events
  *
  * \brief defines the Event class
  *
  * \author Stephen Pietrowicz, NCSA
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

class Event
{
public:

    static const std::string TYPE;
    static const std::string EVENTTIME;
    static const std::string HOSTID;
    static const std::string RUNID;
    static const std::string STATUS;
    static const std::string TOPIC;
    static const std::string PUBTIME;

    Event();
    Event(const std::string& runid, const PropertySet::Ptr ps);
    Event(const std::string& runid, const PropertySet& ps);
    Event(cms::TextMessage *msg);
    // Event(const std::string& runid, const PropertySet::Ptr& ps);

    ~Event();

    // StringArray getCustomPropertyNames()
    PropertySet::Ptr getPropertySet() const;

    std::string getPubDate();
    long long getPubTime();
    void setPubTime(long long t);

    long long getEventTime();
    std::string getEventDate();

    std::string getHostId();
    std::string getRunId();
    std::string getType();
    std::string getStatus();

    void setTopic(std::string topic);
    std::string getTopic();

    vector<std::string> getFilterablePropertyNames();
    vector<std::string> getCustomPropertyNames();
    PropertySet::Ptr getCustomPropertySet() const;
    virtual void populateHeader(cms::TextMessage* msg);
    void marshall(cms::TextMessage *msg);



protected:
    PropertySet::Ptr _psp;
    set<std::string> _keywords;
    void _init();
    void _constructor(const std::string& runid, const PropertySet& ps);
    void splitString(std::string str, std::string delim, std::vector<std::string>&results);

private:
    std::string marshall(const PropertySet& ps);
    PropertySet::Ptr processTextMessage(cms::TextMessage *textMessage);
    PropertySet::Ptr unmarshall(const std::string& text);
};

}
}
}


#endif /*end LSST_CTRL_EVENTS_EVENT_H*/
