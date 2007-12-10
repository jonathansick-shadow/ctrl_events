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

EventLog::EventLog(bool verbose, int threshold, const vector<shared_ptr<DataProperty> > *preamble) 
    :  Log(threshold)
{
    _formatter = new EventFormatter(verbose);
    shared_ptr<LogFormatter> fmtr(_formatter);

    _log = new LogDestination(&clog, fmtr, INHERIT_THRESHOLD);
    shared_ptr<LogDestination> dest(_log);
    _destinations.push_back(dest);

    if (preamble != 0) 
        _preamble.insert(_preamble.end(), preamble->begin(), preamble->end());
}

EventLog::~EventLog() {
}

}}  // end lsst::events
