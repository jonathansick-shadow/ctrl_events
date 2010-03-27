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
#include <time.h>
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

// NOTE:  While it would be nice to implement this as a wrapper for a TextMessage,
//        the JMS/ActiveMQ API doesn't have straight constructors Message objects.
//        They are all created through a JMS/ActiveMQ Session object.  This
//        makes implementation of some methods (like getFilterablePropertyNames)
//        more than just a straight call to a JMS method, since there is no
//        underlying Message wrapped.

Event::Event() {
    _init();
}

void Event::_init() {
    _keywords.push_back("TYPE");
    _keywords.push_back("EVENTTIME");
    _keywords.push_back("HOSTID");
    _keywords.push_back("RUNID");
    _keywords.push_back("STATUS");

    _keywords.push_back("TOPIC");
    _keywords.push_back("PUBTIME");
}

Event::Event(cms::TextMessage *msg, const PropertySet::Ptr psp) {
    _init();

    _psp = psp;

    _type = msg->getStringProperty("TYPE");
    _eventTime = msg->getLongProperty("EVENTTIME") ;
    _hostId = msg->getStringProperty("HOSTID") ;
    _runId = msg->getStringProperty("RUNID") ;
    _status = msg->getStringProperty("STATUS") ;

    _topic = msg->getStringProperty("TOPIC") ;
    _pubTime = msg->getLongProperty("PUBTIME") ;

    _psp->set("TYPE", _type);
    _psp->set("EVENTTIME", _eventTime);
    _psp->set("HOSTID", _hostId);
    _psp->set("RUNID", _runId);
    _psp->set("STATUS", _status);
    _psp->set("TOPIC", _topic);
}

vector<std::string> Event::getFilterablePropertyNames() {
    return _keywords;
}

vector<std::string> Event::getCustomPropertyNames() {
    vector<std::string> names = _psp->names();

    vector<std::string>::iterator nameIterator;
    vector<std::string>::iterator keyIterator;

    for (keyIterator = _keywords.begin(); keyIterator != _keywords.end(); keyIterator++) {
        for (nameIterator = names.begin(); nameIterator != names.end();) {
                std::string key = *keyIterator;
                std::string name = *nameIterator;
                if (key.compare(name) == 0)
                    names.erase(nameIterator);
                else
                    nameIterator++;
        }
    }
    return names;
}

Event::Event( const std::string& runId, const PropertySet::Ptr psp) {
    char hostname[HOST_NAME_MAX];
    time_t rawtime;

    _init();
    
    _psp = psp->deepCopy();

    if (!_psp->exists("STATUS"))
        throw LSST_EXCEPT(pexExceptions::NotFoundException, "'STATUS' not found in PropertySet");
    else {
        _status = _psp->get<std::string>("STATUS");
    }

    if (!_psp->exists("EVENTTIME"))
        _eventTime = time(&rawtime); // current time in ns
    else {
        _eventTime = _psp->get<long>("EVENTTIME");
    }

    if (!psp->exists("HOSTID")) {
       gethostname(hostname, HOST_NAME_MAX);
       _hostId = hostname;
    } else {
       _hostId = _psp->get<std::string>("HOSTID");
    }

    // _runId is filled in here and is ignored in the passed PropertySet
    _runId = runId;

    // _type is filled in here and is ignored in the passed PropertySet
    _type = "_event"; // EventTypes::EVENT;

    // _topic is filled in on publish and is ignored in the passed PropertySet
    _topic = "meps";

    // _pubTime is filled in on publish and is ignored in the passed PropertySet
    _pubTime = 0L;

    // do NOT alter the property set we were given. Make a copy of it,
    // and modify that one.
    // PropertySet ps = new PropertySet(psp);

    _psp = psp->deepCopy();
    
    setKeywords();
}

void Event::setKeywords() {
    _psp->set("TYPE", _type);
    _psp->set("TOPIC", _topic);
    _psp->set("EVENTTIME", _eventTime);
    _psp->set("HOSTID", _hostId);
    _psp->set("RUNID", _runId);
    _psp->set("STATUS", _status);
    _psp->set("PUBTIME", _pubTime);
}

void Event::populateHeader(cms::TextMessage* msg) const {

    msg->setStringProperty("TYPE", _type);
    msg->setStringProperty("TOPIC", _topic);
    msg->setLongProperty("EVENTTIME", _eventTime);
    msg->setStringProperty("HOSTID", _hostId);
    msg->setStringProperty("RUNID", _runId);
    msg->setStringProperty("STATUS", _status);
    msg->setLongProperty("PUBTIME", _pubTime);
}

long Event::getEventTime() {
    return _eventTime;
}

/** \brief Get the creation date of this event
  * \return A formatted date string representing the event creation time
  */
std::string Event::getEventDate() {
    return std::string(ctime(&_eventTime));
}


PropertySet::Ptr Event::getPropertySet() const {
    return _psp;
}

void Event::setPubTime(long t) {
    _pubTime = t;
}

long Event::getPubTime() {
    return _pubTime;
}

/** \brief Get the publication date of this event
  * \return A formatted date string represeting the publication time
  */
std::string Event::getPubDate() {
    if (_pubTime == 0L)
        return std::string();
    return std::string(ctime(&_pubTime));
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



/** \brief destructor
  */
Event::~Event() {
}

}
}
}
