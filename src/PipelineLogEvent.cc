
// -*- lsst-c++ -*-
/** \file PipelineLogEvent.cc
  *
  * \brief Log Event implementation
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

#include "lsst/ctrl/events/EventTypes.h"
#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/LogEvent.h"
#include "lsst/ctrl/events/PipelineLogEvent.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/pex/exceptions.h"

namespace pexExceptions = lsst::pex::exceptions;
namespace pexLogging = lsst::pex::logging;


using namespace std;

namespace lsst {
namespace ctrl {
namespace events {

const std::string PipelineLogEvent::DATAID = "DATAID";
const std::string PipelineLogEvent::LOOPNUM = "LOOPNUM";
const std::string PipelineLogEvent::PIPELINE = "PIPELINE";
const std::string PipelineLogEvent::SLICEID = "SLICEID";
const std::string PipelineLogEvent::STAGEID = "STAGEID";

/** \brief Creates PipelineLogEvent which contains a PropertySet
  *
  */

PipelineLogEvent::PipelineLogEvent() : LogEvent() {
    _init();
}


void PipelineLogEvent::_init() {
    _keywords.insert(PipelineLogEvent::DATAID);
    _keywords.insert(PipelineLogEvent::LOOPNUM);
    _keywords.insert(PipelineLogEvent::PIPELINE);
    _keywords.insert(PipelineLogEvent::SLICEID);
    _keywords.insert(PipelineLogEvent::STAGEID);
}

PipelineLogEvent::PipelineLogEvent(cms::TextMessage *msg) : LogEvent(msg) {
    
    _init();

    _psp->set(PipelineLogEvent::DATAID, msg->getStringProperty(PipelineLogEvent::DATAID));
    _psp->set(PipelineLogEvent::LOOPNUM, msg->getIntProperty(PipelineLogEvent::LOOPNUM));
    _psp->set(PipelineLogEvent::PIPELINE, msg->getStringProperty(PipelineLogEvent::PIPELINE));
    _psp->set(PipelineLogEvent::SLICEID, msg->getIntProperty(PipelineLogEvent::SLICEID));
    _psp->set(PipelineLogEvent::STAGEID, msg->getIntProperty(PipelineLogEvent::STAGEID));

}

PipelineLogEvent::PipelineLogEvent( const std::string& runId, const pexLogging::LogRecord& rec) : LogEvent(runId, rec) {
    _init();


    const PropertySet& ps = rec.getProperties();

    _psp->set(TYPE, EventTypes::PIPELINELOG);

    if (!ps.exists(PipelineLogEvent::DATAID))
        _psp->set(PipelineLogEvent::DATAID, "unknown");
    else
        _psp->set(PipelineLogEvent::DATAID, ps.getAsString(PipelineLogEvent::DATAID));

    if (!ps.exists(PipelineLogEvent::LOOPNUM))
        _psp->set(PipelineLogEvent::LOOPNUM, -1);
    else
        _psp->set(PipelineLogEvent::LOOPNUM, ps.getAsInt(PipelineLogEvent::LOOPNUM));


    if (!ps.exists(PipelineLogEvent::PIPELINE)) {
        _psp->set(PipelineLogEvent::PIPELINE, "unknown");
    } else
         _psp->set(PipelineLogEvent::PIPELINE, ps.getAsString(PipelineLogEvent::PIPELINE));

    if (!ps.exists(PipelineLogEvent::SLICEID))
        _psp->set(PipelineLogEvent::SLICEID, -1);
    else
        _psp->set(PipelineLogEvent::SLICEID, ps.getAsInt(PipelineLogEvent::SLICEID));

    if (!ps.exists(PipelineLogEvent::STAGEID))
        _psp->set(PipelineLogEvent::STAGEID, -1);
    else
        _psp->set(PipelineLogEvent::STAGEID, ps.getAsInt(PipelineLogEvent::STAGEID));

}

void PipelineLogEvent::populateHeader(cms::TextMessage* msg) const {
    LogEvent::populateHeader(msg);

    msg->setStringProperty(PipelineLogEvent::DATAID, _psp->get<std::string>(PipelineLogEvent::DATAID));
    msg->setIntProperty(PipelineLogEvent::LOOPNUM, _psp->get<int>(PipelineLogEvent::LOOPNUM));
    msg->setStringProperty(PipelineLogEvent::PIPELINE, _psp->get<std::string>(PipelineLogEvent::PIPELINE));
    msg->setIntProperty(PipelineLogEvent::SLICEID, _psp->get<int>(PipelineLogEvent::SLICEID));
    msg->setIntProperty(PipelineLogEvent::STAGEID, _psp->get<int>(PipelineLogEvent::STAGEID));
}

std::string PipelineLogEvent::getDataId() {
    return _psp->get<std::string>(PipelineLogEvent::DATAID);
}

int PipelineLogEvent::getLoopnum() {
    return _psp->get<int>(PipelineLogEvent::LOOPNUM);
}

std::string PipelineLogEvent::getPipeline() {
    return _psp->get<std::string>(PipelineLogEvent::PIPELINE);
}

int PipelineLogEvent::getSliceId() {
    return _psp->get<int>(PipelineLogEvent::SLICEID);
}

int PipelineLogEvent::getStageId() {
    return _psp->get<int>(PipelineLogEvent::STAGEID);
}

/** \brief destructor
  */
PipelineLogEvent::~PipelineLogEvent() {
}

}
}
}
