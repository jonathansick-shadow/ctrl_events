// -*- lsst-c++ -*-
/** \file EventLog.cc
  *
  * \brief LSST event logging class
  *
  * This class supports 
  *
  * \ingroup events
  *
  * \author Stephen R. Pietrowicz, NCSA
  *
  */
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <unistd.h>

#include "lsst/ctrl/events/EventLog.h"
#include "lsst/ctrl/events/EventFormatter.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/logging/Component.h"

using namespace lsst::daf::base;
using namespace boost;

using namespace std;

// this is required because Mac doesn't define this in the standard place
// (or where they said it was in the man pages)
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 255
#endif

namespace lsst {
namespace ctrl {
namespace events {

/** \brief constructor for EventLog.   
  * \param runId name of the run
  * \param sliceId the current slice id
  * \param hostId the name for this host.
  * \param threshold threshold  of this log. Default is threshold is Log::INFO
  */
EventLog::EventLog(const std::string runId, int sliceId, const std::string hostId, int threshold) 
    :  Log(threshold)
{
    PropertySet::Ptr psp;
    init(runId, sliceId, psp, hostId, threshold);
}

/** \brief constructor for EventLog.   
  * \param runId name of the run
  * \param sliceId the current slice id
  * \param preamble a list of PropertySet to include in each log message.
  * \param hostId the name for this host.
  * \param threshold threshold  of this log. Default is threshold is Log::INFO
  */
EventLog::EventLog(const std::string runId, int sliceId, const PropertySet::Ptr &preamble, const std::string hostId, int threshold) 
    :  Log(threshold)
{
    init(runId, sliceId, preamble, hostId, threshold);
}

/** private method to initialize from each constructor
  */
void EventLog::init(const std::string runId, int sliceId, const PropertySet::Ptr &preamble, const std::string hostId, int threshold) 
{

    initThres(threshold);
    char host[HOST_NAME_MAX];

    // if there is no host name specified, make "unknown host" the name
    if (hostId.size() == 0) {
        if (gethostname(host, HOST_NAME_MAX) < 0)
            strcpy(host, "unknown host");
    } else
        strcpy(host, hostId.c_str());

    std::string hostName(host);

    // TODO:  add preamble to _preamble
    if (preamble.get() != 0)
        _preamble = preamble->deepCopy();

    _preamble->add("hostId", hostName);
    _preamble->add("runId", runId);
    _preamble->add("sliceId", sliceId);
}

/** private method to initialize the logging mechanism
  */
void EventLog::initThres(int threshold)
{
    _formatter = new EventFormatter();
    shared_ptr<LogFormatter> fmtr(_formatter);

    _log = new LogDestination(&clog, fmtr, INHERIT_THRESHOLD);
    shared_ptr<LogDestination> dest(_log);
    _destinations.push_back(dest);
}

/** \brief creates a default log
  * \param runId name of the run
  * \param sliceId the current slice id
  * \param hostId the name of this host
  * \param threshold the logging threshold to observe when sending log messages
  * \param preamble a PropertySet to include in each log message.
  */
void EventLog::createDefaultLog(const std::string runId, int sliceId, const std::string hostId, int threshold)  {
    PropertySet::Ptr psp;
    Log::setDefaultLog(new EventLog(runId, sliceId, psp, hostId, threshold));
}

void EventLog::createDefaultLog(const std::string runId, int sliceId, const PropertySet::Ptr& preamble, const std::string hostId, int threshold)  {
    Log::setDefaultLog(new EventLog(runId, sliceId, preamble, hostId, threshold));
}

/** \brief destructor
  */
EventLog::~EventLog() {
}

}}}  // end lsst::ctrl::events
