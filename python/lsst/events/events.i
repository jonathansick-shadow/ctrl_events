/* Events.i */
%module("directors=1") events

%{
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include "lsst/mwi/data/Citizen.h"
#include "lsst/mwi/exceptions.h"
#include "lsst/mwi/policy/Policy.h"
#include "lsst/mwi/data/DataProperty.h"
#include "lsst/events/Events.h"
#include "lsst/events/EventSystem.h"
#include "lsst/mwi/logging/Log.h"
#include "lsst/mwi/logging/LogRecord.h"
#include "lsst/events/EventLog.h"
#include "lsst/events/EventFormatter.h"
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
using namespace lsst::mwi::data;
using namespace lsst::mwi::policy;
%}

%init %{
%}
%pythoncode %{
import lsst.mwi.data
import lsst.mwi.policy
%}

%include "std_string.i"
%include "std_set.i"
%include "lsst/mwi/p_lsstSwig.i"
%include "lsst/mwi/utils/Utils.h"
%import "lsst/mwi/data/Citizen.h"
%import "lsst/mwi/policy/Policy.h"
%import "lsst/mwi/data/DataProperty.h"
%import "lsst/mwi/exceptions.h"
%include "lsst/mwi/logging/Log.h"
%import "lsst/mwi/logging/LogRecord.h"
%include "lsst/events/Events.h"
%include "lsst/events/EventLog.h"
%include "lsst/events/EventFormatter.h"
%include "lsst/events/EventSystem.h"
