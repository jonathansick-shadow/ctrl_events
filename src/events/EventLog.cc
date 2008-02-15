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

#include "lsst/events/EventLog.h"
#include "lsst/events/EventFormatter.h"
#include "lsst/events/EventSystem.h"
#include "lsst/mwi/data/DataProperty.h"
#include "lsst/mwi/data/SupportFactory.h"
#include "lsst/mwi/utils/Component.h"

using namespace lsst::mwi::data;
using namespace boost;

using namespace std;

// this is to get around a Mac compile error since Mac didn't have HOST_NAME_MAX defined
// in the standard place
#define LSST_HOST_NAME_MAX 128

namespace lsst {
namespace events {

/** \brief constructor for EventLog.   
  * \param runId name of the run
  * \param sliceId the current slice id
  * \param hostId the name for this host.
  * \param threshold threshold  of this log. Default is threshold is Log::INFO
  * \param preamble a list of DataProperty to include in each log message.
  */
EventLog::EventLog(const std::string runId, int sliceId, const std::string hostId, int threshold, const list<shared_ptr<DataProperty> > *preamble) 

    :  Log(threshold)
{

    init(threshold);
    char host[LSST_HOST_NAME_MAX];

    // if there is no host name specified, make "unknown host" the name
    if (hostId.size() == 0) {
        if (gethostname(host, LSST_HOST_NAME_MAX) < 0)
            strcpy(host, "unknown host");
    } else
        strcpy(host, hostId.c_str());

    std::string hostName(host);


    if (preamble != 0) 
        _preamble.insert(_preamble.end(), preamble->begin(), preamble->end());

    _preamble.insert(_preamble.end(),            
                  DataProperty::PtrType(new DataProperty("hostId", hostName)));
    _preamble.insert(_preamble.end(), 
                  DataProperty::PtrType(new DataProperty("runId", runId)));
    _preamble.insert(_preamble.end(),
                  DataProperty::PtrType(new DataProperty("sliceId", sliceId)));
}

/** private method to initialize the logging mechanism
  */
void EventLog::init(int threshold)
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
  * \param preamble a list of DataPropertys to include in each log message.
  */
void EventLog::createDefaultLog(const std::string runId, int sliceId, const std::string hostId, int threshold, const list<shared_ptr<DataProperty> > *preamble)  {
    Log::setDefaultLog(new EventLog(runId, sliceId, hostId, threshold, preamble));
}

/** \brief destructor
  */
EventLog::~EventLog() {
}

}}  // end lsst::events
