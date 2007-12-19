// -*- lsst-c++ -*-
/** \file EventLog.cc
  *
  * \brief 
  *
  * Author: Stephen R. Pietrowicz, NCSA
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

namespace lsst {
namespace events {
/**
  * \brief constructor for EventLog.   
  * \param runId name of the run
  * \param sliceId the current slice id
  * \param hostId the identifier for this host.  Default is 0
  * \param threshold threshold  of this log. Default is threshold is Log::INFO
  * \param preamble a list of DataProperty to include in each log message.  Default is 0
  */
EventLog::EventLog(const std::string runId, int sliceId, const std::string hostId, int threshold, const list<shared_ptr<DataProperty> > *preamble) 

    :  Log(threshold)
{
    init(threshold);
    char host[HOST_NAME_MAX];

    if (hostId.size() == 0) {
        if (gethostname(host, HOST_NAME_MAX) < 0)
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

#ifdef NOTDEF
EventLog::EventLog(int threshold, const list<shared_ptr<DataProperty> > *preamble) 
    :  Log(threshold)
{
    init(threshold);
    if (preamble != 0) 
        _preamble.insert(_preamble.end(), preamble->begin(), preamble->end());
}
#endif

void EventLog::init(int threshold)
{
    _formatter = new EventFormatter();
    shared_ptr<LogFormatter> fmtr(_formatter);

    _log = new LogDestination(&clog, fmtr, INHERIT_THRESHOLD);
    shared_ptr<LogDestination> dest(_log);
    _destinations.push_back(dest);
}

void EventLog::createDefaultLog(const std::string runId, int sliceId, const std::string hostId, int threshold, const list<shared_ptr<DataProperty> > *preamble)  {
    Log::setDefaultLog(new EventLog(runId, sliceId, hostId, threshold, preamble));
}

EventLog::~EventLog() {
}

}}  // end lsst::events
