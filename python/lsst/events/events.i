/* Events.i */
%module("directors=1") events

%{
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include "lsst/daf/data/Citizen.h"
#include "lsst/pex/exceptions.h"
#include "lsst/pex/policy/Policy.h"
#include "lsst/daf/data/DataProperty.h"
#include "lsst/ctrl/events/EventTransmitter.h"
#include "lsst/ctrl/events/EventReceiver.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/pex/logging/Log.h"
#include "lsst/pex/logging/LogRecord.h"
#include "lsst/ctrl/events/EventLog.h"
#include "lsst/ctrl/events/EventFormatter.h"
%}

%inline %{
namespace lsst { namespace mwi { namespace data { } } } 
namespace lsst { namespace mwi { namespace policy { } } } 
namespace activemq { namespace core { } } 
namespace activemq { namespace util { } } 
namespace activemq { namespace concurrent { } } 
namespace cms { } 
namespace boost { namespace filesystem {} }

using namespace lsst;
using namespace lsst::daf::data;
using namespace lsst::pex::policy;
%}

%init %{
%}
%pythoncode %{
import lsst.daf.data
import lsst.pex.policy
%}

%include "std_string.i"
%include "std_set.i"
%include "lsst/mwi/p_lsstSwig.i"
%include "lsst/pex/utils/Utils.h"
%import "lsst/daf/data/Citizen.h"
%import "lsst/pex/policy/Policy.h"
%import "lsst/daf/data/DataProperty.h"
%import "lsst/pex/exceptions.h"
%import "lsst/pex/logging/Log.h"
%import "lsst/pex/logging/LogRecord.h"
%include "lsst/ctrl/events/EventTransmitter.h"
%include "lsst/ctrl/events/EventReceiver.h"
%include "lsst/ctrl/events/EventLog.h"
%include "lsst/ctrl/events/EventFormatter.h"
%include "lsst/ctrl/events/EventSystem.h"
