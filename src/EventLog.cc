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
#include <cstring>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <unistd.h>

#include "lsst/ctrl/events/EventLog.h"
#include "lsst/ctrl/events/EventFormatter.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/logging/Component.h"

namespace pexLogging = lsst::pex::logging;

using namespace std;

namespace lsst {
namespace ctrl {
namespace events {

const std::string EventLog::LOGGING_TOPIC = "logging";

/** \brief constructor for EventLog.   
  * \param runId name of the run
  * \param sliceId the current slice id
  * \param hostId the name for this host.
  * \param threshold threshold  of this log. Default is threshold is Log::INFO
  */
EventLog::EventLog(const std::string runId, int sliceId, const std::string hostId, int threshold) 
    :  pexLogging::Log(threshold)
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
    long int host_len = sysconf(_SC_HOST_NAME_MAX);

    boost::scoped_array<char> host(new char[host_len]);

    // if there is no host name specified, make "unknown host" the name
    if (hostId.size() == 0) {
        if (gethostname(host.get(), host_len) < 0)
            std::strcpy(host.get(), "unknown host");
    } else
        std::strcpy(host.get(), hostId.c_str());

    std::string hostName(host.get());

    // TODO:  add preamble to _preamble
    if (preamble.get() != 0)
        _preamble = preamble->deepCopy();

    // this will be added by the Event object
    // _preamble->add("hostId", hostName);
    _preamble->add("RUNID", runId);
    _preamble->add("SLICEID", sliceId);
}

/** private method to initialize the logging mechanism
  */
void EventLog::initThres(int threshold)
{
    _formatter = new EventFormatter();
    boost::shared_ptr<pexLogging::LogFormatter> fmtr(_formatter);

    _log = new pexLogging::LogDestination(&clog, fmtr, INHERIT_THRESHOLD);
    boost::shared_ptr<pexLogging::LogDestination> dest(_log);
    _destinations.push_back(dest);
}

/** \brief creates a default log
  * \param runId name of the run
  * \param sliceId the current slice id
  * \param hostId the name of this host
  * \param threshold the logging threshold to observe when sending log messages
  */
void EventLog::createDefaultLog(const std::string runId, int sliceId, const std::string hostId, int threshold)  {
    PropertySet::Ptr psp;
    pexLogging::Log::setDefaultLog(new EventLog(runId, sliceId, psp, hostId, threshold));
}

void EventLog::createDefaultLog(const std::string runId, int sliceId, const PropertySet::Ptr& preamble, const std::string hostId, int threshold)  {
    pexLogging::Log::setDefaultLog(new EventLog(runId, sliceId, preamble, hostId, threshold));
}

/** \brief destructor
  */
EventLog::~EventLog() {
}

}}}  // end lsst::ctrl::events
