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
/* #include "lsst/ctrl/events/EventTypes.h" */
#include "lsst/ctrl/events/EventTransmitter.h"
#include "lsst/ctrl/events/EventReceiver.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/ctrl/events/EventLog.h"
#include "lsst/ctrl/events/EventFormatter.h"

%}

%include "lsst/p_lsstSwig.i"
%lsst_exceptions()

SWIG_SHARED_PTR_DERIVED(EventFormatter, lsst::pex::logging::LogFormatter, lsst::ctrl::events::EventFormatter)

%import "lsst/daf/base/baseLib.i"
%import "lsst/pex/logging/loggingLib.i"
%import "lsst/pex/policy/policyLib.i"
/*
*/

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
%}
%nothread lsst::ctrl::events::EventTransmitter::EventTransmitter;
%nothread lsst::ctrl::events::EventSystem::createTransmitter;
%nothread lsst::ctrl::events::EventSystem::createLocalTransmitter;
%nothread lsst::ctrl::events::EventSystem::createLocalReceiver;

%include "lsst/ctrl/events/Event.h"
%include "lsst/ctrl/events/StatusEvent.h"
/* %include "lsst/ctrl/events/EventTypes.h" */
%include "lsst/ctrl/events/EventTransmitter.h"
%include "lsst/ctrl/events/EventReceiver.h"
%include "lsst/ctrl/events/EventLog.h"
%include "lsst/ctrl/events/EventFormatter.h"
%include "lsst/ctrl/events/EventSystem.h"


/******************************************************************************/
// Local Variables: ***
// eval: (setq indent-tabs-mode nil) ***
// End: ***

