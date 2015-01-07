// -*- lsst-c++ -*-

/* 
 * LSST Data Management System
 * Copyright 2008, 2009, 2010 LSST Corporation.
 * 
 * This product includes software developed by the
 * LSST Project (http://www.lsst.org/).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the LSST License Statement and 
 * the GNU General Public License along with this program.  If not, 
 * see <http://www.lsstcorp.org/LegalNotices/>.
 */
 

/* Events.i */
%define eventsLib_DOCSTRING
"
Access to the lsst::ctrl::events classes
"
%enddef

%feature("autodoc", "1");
%module(package="lsst.ctrl.events", docstring=eventsLib_DOCSTRING) eventsLib



%{
#include "lsst/daf/base.h"
/* swig pulls in references to ScreenLog.h and DualLog.h for some reason, so that's why these are here */
#include "lsst/pex/logging.h"
#include "lsst/pex/logging/BlockTimingLog.h"
#include "lsst/pex/logging/ScreenLog.h"
#include "lsst/pex/logging/DualLog.h"
#include "lsst/pex/policy.h"
#include "lsst/ctrl/events/OriginatorID.h"
#include "lsst/ctrl/events/DestinationID.h"
#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/StatusEvent.h"
#include "lsst/ctrl/events/CommandEvent.h"
#include "lsst/ctrl/events/LogEvent.h"
#include "lsst/ctrl/events/EventTypes.h"
#include "lsst/ctrl/events/EventTransmitter.h"
#include "lsst/ctrl/events/EventReceiver.h"
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/ctrl/events/EventLog.h"
#include "lsst/ctrl/events/EventFormatter.h"

%}

%include "lsst/p_lsstSwig.i"

%shared_ptr(lsst::ctrl::events::EventFormatter)

%import "lsst/daf/base/baseLib.i"
%import "lsst/pex/logging/loggingLib.i"
%import "lsst/pex/policy/policyLib.i"

%include "lsst/ctrl/events/OriginatorID.h"
%include "lsst/ctrl/events/DestinationID.h"
%include "lsst/ctrl/events/Event.h"
%include "lsst/ctrl/events/StatusEvent.h"
%include "lsst/ctrl/events/CommandEvent.h"
%include "lsst/ctrl/events/LogEvent.h"
%include "lsst/ctrl/events/EventTypes.h"
%include "lsst/ctrl/events/EventTransmitter.h"

%newobject lsst::ctrl::events::EventReceiver::receiveEvent;
%newobject lsst::ctrl::events::EventSystem::receiveEvent;
%newobject lsst::ctrl::events::EventReceiver::receiveStatusEvent;
%newobject lsst::ctrl::events::EventReceiver::receiveCommandEvent;
%newobject lsst::ctrl::events::EventReceiver::receiveLogEvent;
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
    lsst::ctrl::events::LogEvent *receiveLogEvent() {
        return dynamic_cast<lsst::ctrl::events::LogEvent*>(self->receiveEvent());
    }
    lsst::ctrl::events::LogEvent *receiveLogEvent(long timeout) {
        return dynamic_cast<lsst::ctrl::events::LogEvent*>(self->receiveEvent(timeout));
    }
}


/******************************************************************************/
// Local Variables: ***
// eval: (setq indent-tabs-mode nil) ***
// End: ***

