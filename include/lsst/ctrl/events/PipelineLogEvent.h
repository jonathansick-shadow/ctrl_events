
// -*- lsst-c++ -*-
/** \file PipelineLogEvent.h
  *
  * \ingroup events
  *
  * \brief defines the PipelineLogEvent class
  *
  * \author Stephen Pietrowicz, NCSA
  *
  */

#ifndef LSST_CTRL_EVENTS_PIPELINELOGEVENT_H
#define LSST_CTRL_EVENTS_PIPELINELOGEVENT_H

#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/BytesMessage.h>

#include <stdlib.h>
#include <iostream>

#include "lsst/ctrl/events/Event.h"
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

class PipelineLogEvent : public LogEvent
{
public:
    static const std::string DATAID;
    static const std::string LOOPNUM;
    static const std::string PIPELINE;
    static const std::string SLICEID;
    static const std::string STAGEID;

    PipelineLogEvent();
    PipelineLogEvent(const std::string& runid, const pexLogging::LogRecord& rec);
    PipelineLogEvent(cms::TextMessage *msg);

    ~PipelineLogEvent();

    void populateHeader(cms::TextMessage *msg);

    std::string getDataId();
    int getLoopnum();
    std::string getPipeline();
    int getSliceId();
    int getStageId();

private:

    void _init();

};
}
}
}


#endif /*end LSST_CTRL_EVENTS_PIPELINELOGEVENT_H*/
