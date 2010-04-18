// -*- lsst-c++ -*-

/* Events.i */
%define eventsLib_DOCSTRING
"
Access to the lsst::ctrl::events classes
"
%enddef

%feature("autodoc", "1");
%module(package="lsst.ctrl.events", docstring=eventsLib_DOCSTRING) eventsLib



%{
/* swig pulls in references to ScreenLog.h and DualLog.h for some reason, so that's why these are here */
#include "lsst/pex/logging/ScreenLog.h"
#include "lsst/pex/logging/Debug.h"
#include "lsst/pex/logging/DualLog.h"
#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/StatusEvent.h"
#include "lsst/ctrl/events/CommandEvent.h"
#include "lsst/ctrl/events/EventTransmitter.h"
#include "lsst/ctrl/events/EventReceiver.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/ctrl/events/EventLog.h"
#include "lsst/ctrl/events/EventFormatter.h"

%}

%include "lsst/p_lsstSwig.i"

SWIG_SHARED_PTR_DERIVED(EventFormatter, lsst::pex::logging::LogFormatter, lsst::ctrl::events::EventFormatter)

%import "lsst/daf/base/baseLib.i"
%import "lsst/pex/logging/loggingLib.i"
%import "lsst/pex/policy/policyLib.i"

%inline %{
namespace lsst {
    namespace pex {
        namespace logging {}
    }
}
namespace lsst {
    namespace pex {
        namespace policy {}
    }
}
#if LONG_MAX > 2200000000
/* 64-bit machine */
typedef long int int64_t;
#else
/* 32-bit machine */
typedef long long int64_t;
#endif
%}

/* exception definitions for this package have to reaquire the GIL
 * or we get segmentation faults.  Python expects to have the GIL  when
 * we get back into it.
 */
%exception {
    try {
        $action
    } catch (lsst::pex::exceptions::Exception &e) {
        SWIG_PYTHON_THREAD_END_ALLOW;
        SWIG_PYTHON_THREAD_END_BLOCK;
        raiseLsstException(e);
        SWIG_fail;
    } catch (std::exception & e) {
        SWIG_PYTHON_THREAD_END_ALLOW;
        SWIG_PYTHON_THREAD_END_BLOCK;
        PyErr_SetString(PyExc_Exception, e.what());
        SWIG_fail;
    }
}

%include "lsst/ctrl/events/Event.h"
%include "lsst/ctrl/events/StatusEvent.h"
%include "lsst/ctrl/events/CommandEvent.h"
%include "lsst/ctrl/events/EventTransmitter.h"
%include "lsst/ctrl/events/EventReceiver.h"
%include "lsst/ctrl/events/EventLog.h"
%include "lsst/ctrl/events/EventFormatter.h"
%include "lsst/ctrl/events/EventSystem.h"

%extend lsst::ctrl::events::EventReceiver {
    lsst::ctrl::events::StatusEvent *receiveStatusEvent() {
        return dynamic_cast<lsst::ctrl::events::StatusEvent*>(self->receiveEvent());
    }
    lsst::ctrl::events::StatusEvent *receiveStatusEvent(long timeout) {
        return dynamic_cast<lsst::ctrl::events::StatusEvent*>(self->receiveEvent(timeout));
    }
    lsst::ctrl::events::CommandEvent *receiveCommandEvent() {
        return dynamic_cast<lsst::ctrl::events::CommandEvent*>(self->receiveEvent());
    }
    lsst::ctrl::events::CommandEvent *receiveCommandEvent(long timeout) {
        return dynamic_cast<lsst::ctrl::events::CommandEvent*>(self->receiveEvent(timeout));
    }
}


/******************************************************************************/
// Local Variables: ***
// eval: (setq indent-tabs-mode nil) ***
// End: ***

