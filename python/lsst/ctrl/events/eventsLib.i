// -*- lsst-c++ -*-

/* Events.i */
%define eventsLib_DOCSTRING
"
Access to the lsst::ctrl::events classes
"
%enddef

%feature("autodoc", "1");
%module(package="lsst::ctrl::events", docstring=eventsLib_DOCSTRING, "directors=1") eventsLib

%{
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include "lsst/daf/base/Citizen.h"
#include "lsst/pex/exceptions.h"
#include "lsst/pex/policy/Policy.h"
#include "lsst/daf/base/PropertySet.h"
#include "lsst/ctrl/events/EventTransmitter.h"
#include "lsst/ctrl/events/EventReceiver.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/pex/logging/Log.h"
#include "lsst/pex/logging/LogRecord.h"
#include "lsst/ctrl/events/EventLog.h"
#include "lsst/ctrl/events/EventFormatter.h"
%}

%inline %{
namespace lsst { namespace ctrl { namespace events { } } }
namespace lsst { namespace daf { namespace base { } } } 
namespace lsst { namespace pex { namespace policy { } } } 
namespace activemq { namespace core { } } 
namespace activemq { namespace util { } } 
namespace activemq { namespace concurrent { } } 
namespace cms { } 
namespace boost { namespace filesystem {} }

using namespace lsst;
using namespace lsst::ctrl::events;
using namespace lsst::daf::base;
using namespace lsst::pex::policy;
%}

%init %{
%}

%pythoncode %{
import lsst.daf.base
import lsst.pex.policy
import lsst.ctrl.events
%}

%include "lsst/p_lsstSwig.i"
%include "std_string.i"
%include "std_set.i"
%include "lsst/utils/Utils.h"
%import "lsst/daf/base/Citizen.h"
%import "lsst/daf/base/PropertySet.h"
%import "lsst/pex/exceptions.h"
%import "lsst/pex/logging/Log.h"
%import "lsst/pex/logging/LogRecord.h"
%import "lsst/pex/policy/Policy.h"


%include "lsst/ctrl/events/EventTransmitter.h"
%include "lsst/ctrl/events/EventReceiver.h"
%include "lsst/ctrl/events/EventLog.h"
%include "lsst/ctrl/events/EventFormatter.h"
%include "lsst/ctrl/events/EventSystem.h"
/*
%extend lsst::ctrl::events::EventSystem {
        %template(matchingReceiveInt) matchingReceive<int>;
        %template(matchingReceiveDouble) matchingReceive<double>;
        %template(matchingReceiveFloat) matchingReceive<float>;
        %template(matchingReceiveString) matchingReceive<std::string>;
}
*/



/******************************************************************************/
// Local Variables: ***
// eval: (setq indent-tabs-mode nil) ***
// End: ***
