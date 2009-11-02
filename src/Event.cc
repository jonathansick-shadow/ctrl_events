// -*- lsst-c++ -*-
/** \file Event.cc
  *
  * \brief Object describing the contents of an Event
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

#include <sys/time.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#include "lsst/ctrl/events/Event.h"
#include "lsst/daf/base/DateTime.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/exceptions.h"
#include "lsst/pex/logging/Component.h"
#include "lsst/pex/policy/Policy.h"
#include "lsst/pex/logging/LogRecord.h"
#include <sys/socket.h>
#include <sys/un.h>

namespace pexExceptions = lsst::pex::exceptions;
namespace pexLogging = lsst::pex::logging;


using namespace std;
using std::numeric_limits;

namespace lsst {
namespace ctrl {
namespace events {

Event::Event( const std::string& runId, const PropertySet::Ptr& psp) {
    _runId = runId;
    _hostId = getIPAddr();
    _status = std::string("undefined");
    _topic = std::string("unpublished");
    _type = std::string("unknown");
    _pubTime = 0L;

    _psp = psp;

    _eventTime = currentTime();
}

Event::~Event() {
}

std::string Event::getHostId() {
    return _hostId;
}

std::string Event::getRunId() {
    return _runId;
}

std::string Event::getStatus() {
    return _status;
}

std::string Event::getType() {
    return std::string("unknown");
}

void Event::getCustomPropertyNames() {
    return;
}

std::string Event::getDate() {
    return NULL;
}

PropertySet::Ptr Event::getPropertySet() {
    return PropertySet::Ptr();
}

std::string Event::getPubDate() {
    return NULL;
}

long Event::getPubTime() {
    return 0;
}

std::string Event::getTopic() {
    return _topic;
}

void Event::setEventTime(long t) {
    _eventTime = t;
}

// private methods

void Event::setMessageType(const std::string& msgType) {
    _type = msgType;
}


long Event::currentTime() {
    // the code below should be replaced to call gethrtime()
    // once we get systems capable of supporting that
    
    struct timeval tv;
    long curTime;

    gettimeofday(&tv,0);
    curTime = (tv.tv_sec *1000000 + tv.tv_usec)*1000;
    return curTime;
}


std::string Event::getIPAddr() {
    char buf[HOST_NAME_MAX];
    struct hostent *ent;
    struct in_addr addr, *p;
    char *val;

    if (gethostname(buf,HOST_NAME_MAX) < 0) {
        return std::string("unknown");
    }

    
    ent = gethostbyname(buf);

    p = (in_addr *)memcpy(&addr,ent->h_addr_list[0],sizeof(struct in_addr));
    val = inet_ntoa(addr);
    return std::string(val);

}

}
}
}
