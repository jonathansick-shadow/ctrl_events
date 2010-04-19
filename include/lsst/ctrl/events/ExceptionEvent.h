// -*- lsst-c++ -*-
/** \file ExceptionEvent.h
  *
  * \ingroup events
  *
  * \brief defines the ExceptionEvent class
  *
  * \author Stephen Pietrowicz, NCSA
  *
  */

#ifndef LSST_CTRL_EVENTS_EXCEPTIONEVENT_H
#define LSST_CTRL_EVENTS_EXCEPTIONEVENT_H

#include "lsst/daf/base/PropertySet.h"

using lsst::daf::base::PropertySet;

namespace pexPolicy = lsst::pex::policy;
namespace pexLogging = lsst::pex::logging;

namespace lsst {
namespace ctrl {
namespace events { 

/**
 * @brief Representation of an LSST ExceptionEvent
 */

class ExceptionEvent :: public Event
{

public:
    static const std::string EXCEPTIONTYPE;
    static const std::string STACKTRACE;

    ExceptionEvent();
    ExceptionEvent(const std::string& runid, const LSSTException ex);
    ExceptionEvent(cms::TextMessage *msg);

    ~LogEvent();

    void populateHeader(cms::TextMessage *msg);

    std::string getExceptionType();
    std::string getStackTrace();

private:

    void _init();

};
}
}
}


#endif /*end LSST_CTRL_EVENTS_EXCEPTIONEVENT_H*/
