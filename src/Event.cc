// -*- lsst-c++ -*-
/** \file Event.cc
  *
  * \brief Object to transmit information through the event framework.
  *
  * \ingroup ctrl/events
  *
  * \author Stephen R. Pietrowicz, NCSA
  *
  */
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <cstring>

#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/EventTypes.h"
#include "lsst/daf/base/DateTime.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/exceptions.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include "lsst/ctrl/events/EventLibrary.h"

#include <activemq/core/ActiveMQConnectionFactory.h>

namespace pexExceptions = lsst::pex::exceptions;
namespace pexLogging = lsst::pex::logging;


using namespace std;
using std::numeric_limits;

namespace lsst {
namespace ctrl {
namespace events {

/** \brief Creates Event which contains a PropertySet
  *
  * \throw throws NotFoundException if expected keywords are missing a property set
  */
Event::Event( const std::string& runId, const PropertySet::Ptr psp) {
    // PropertySet::Ptr internalPsp(new PropertySet);

    char hostname[HOST_NAME_MAX];
    struct hostent *hostEntry;
    if (!psp->exists("status"))
        throw LSST_EXCEPT(pexExceptions::NotFoundException, "'status' not found in PropertySet");
    else {
        _status = psp->get<std::string>("status");
        psp->add("status", _status);
        }

    _eventTime = time(0); // current time in ns
    gethostname(hostname, HOST_NAME_MAX);
    hostEntry = gethostbyname(hostname);
    
    
    _hostId = hostname;
    _runId = runId;
    _type = "_event"; // EventTypes::EVENT;
    // _topic is filled in on publish
    // _pubTime is filled in on publish
    _psp = psp;
    
    _pubTime = 0L;
}

void Event::populateHeader(cms::Message* msg) {
    msg->setStringProperty("TYPE", _type);
    msg->setLongProperty("EVENTTIME", _eventTime);
    msg->setStringProperty("HOSTID", _hostId);
    msg->setStringProperty("RUNID", _runId);
    msg->setStringProperty("STATUS", _status);
}

std::string Event::getDate() {
    return "time";
}

PropertySet::Ptr Event::getPropertySet() {
    return _psp;
}

long Event::getPubTime() {
    return _pubTime;
}

std::string Event::getPubDate() {
    std::string pubDate;
    return pubDate;
}


std::string Event::getHostId() {
    return _hostId;
}

std::string Event::getRunId() {
    return _runId;
}

std::string Event::getType() {
    return _type;
}

std::string Event::getStatus() {
    return _status;
}

void Event::setTopic(std::string topic) {
    _topic = topic;
}

std::string Event::getTopic() {
    return _topic;
}
void Event::setEventTime(long t) {
    _pubTime = t;
}

vector<std::string> Event::getCustomPropertyNames() {
    vector<std::string> names;
    return names;
}

/** \brief destructor
  */
Event::~Event() {
}

}
}
}
