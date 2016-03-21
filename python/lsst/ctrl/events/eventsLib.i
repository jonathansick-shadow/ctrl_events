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
%feature("notabstract") EventTransmitter;

%module(package="lsst.ctrl.events", docstring=eventsLib_DOCSTRING) eventsLib



%{
#include "lsst/daf/base.h"
#include "lsst/ctrl/events/Host.h"
#include "lsst/ctrl/events/LocationId.h"
#include "lsst/ctrl/events/Event.h"
#include "lsst/ctrl/events/StatusEvent.h"
#include "lsst/ctrl/events/CommandEvent.h"
#include "lsst/ctrl/events/LogEvent.h"
#include "lsst/ctrl/events/EventTypes.h"
#include "lsst/ctrl/events/Transmitter.h"
#include "lsst/ctrl/events/EventTransmitter.h"
#include "lsst/ctrl/events/EventEnqueuer.h"
#include "lsst/ctrl/events/Receiver.h"
#include "lsst/ctrl/events/EventReceiver.h"
#include "lsst/ctrl/events/EventDequeuer.h"
#include "lsst/ctrl/events/EventSystem.h"

%}

%include "lsst/p_lsstSwig.i"

%shared_ptr(lsst::ctrl::events::LocationId)
%shared_ptr(lsst::ctrl::events::Event)
%shared_ptr(lsst::ctrl::events::StatusEvent)
%shared_ptr(lsst::ctrl::events::CommandEvent)
%shared_ptr(lsst::ctrl::events::LogEvent)


%import "lsst/daf/base/baseLib.i"

%include log4cxx.i

%typemap(out) std::vector<std::string > {
    int len = ($1).size();
    $result = PyList_New(len);
    for (int i = 0; i < len; i++) {
        PyList_SetItem($result,i,PyString_FromString(($1)[i].c_str()));
    }
}

%include "lsst/ctrl/events/Host.h"
%include "lsst/ctrl/events/LocationId.h"
%include "lsst/ctrl/events/Event.h"
%include "lsst/ctrl/events/StatusEvent.h"
%include "lsst/ctrl/events/CommandEvent.h"
%include "lsst/ctrl/events/LogEvent.h"
%include "lsst/ctrl/events/EventTypes.h"
%include "lsst/ctrl/events/Transmitter.h"
%include "lsst/ctrl/events/EventTransmitter.h"
%include "lsst/ctrl/events/EventEnqueuer.h"

%newobject lsst::ctrl::events::EventReceiver::receiveEvent;
%newobject lsst::ctrl::events::EventSystem::receiveEvent;
%newobject lsst::ctrl::events::EventReceiver::receiveStatusEvent;
%newobject lsst::ctrl::events::EventReceiver::receiveCommandEvent;
%newobject lsst::ctrl::events::EventReceiver::receiveLogEvent;
%include "lsst/ctrl/events/Receiver.h"
%include "lsst/ctrl/events/EventReceiver.h"
%include "lsst/ctrl/events/EventDequeuer.h"
%include "lsst/ctrl/events/EventSystem.h"

%extend lsst::ctrl::events::EventReceiver {
    PTR(lsst::ctrl::events::StatusEvent) receiveStatusEvent() {
        PTR(lsst::ctrl::events::Event) ev = self->receiveEvent();
        return boost::static_pointer_cast<lsst::ctrl::events::StatusEvent>(ev);
    }
    PTR(lsst::ctrl::events::StatusEvent) receiveStatusEvent(long timeout) {
        PTR(lsst::ctrl::events::Event) ev = self->receiveEvent(timeout);
        return boost::static_pointer_cast<lsst::ctrl::events::StatusEvent>(ev);
    }
    PTR(lsst::ctrl::events::CommandEvent) receiveCommandEvent() {
        PTR(lsst::ctrl::events::Event) ev = self->receiveEvent();
        return boost::static_pointer_cast<lsst::ctrl::events::CommandEvent>(ev);
    }
    PTR(lsst::ctrl::events::CommandEvent) receiveCommandEvent(long timeout) {
        PTR(lsst::ctrl::events::Event) ev = self->receiveEvent(timeout);
        return boost::static_pointer_cast<lsst::ctrl::events::CommandEvent>(ev);
    }
    PTR(lsst::ctrl::events::LogEvent) receiveLogEvent() {
        PTR(lsst::ctrl::events::Event) ev = self->receiveEvent();
        return boost::static_pointer_cast<lsst::ctrl::events::LogEvent>(ev);
    }
    PTR(lsst::ctrl::events::LogEvent) receiveLogEvent(long timeout) {
        PTR(lsst::ctrl::events::Event) ev = self->receiveEvent(timeout);
        return boost::static_pointer_cast<lsst::ctrl::events::LogEvent>(ev);
    }
}

%extend lsst::ctrl::events::EventDequeuer {
    PTR(lsst::ctrl::events::StatusEvent) receiveStatusEvent() {
        PTR(lsst::ctrl::events::Event) ev = self->receiveEvent();
        return boost::static_pointer_cast<lsst::ctrl::events::StatusEvent>(ev);
    }
    PTR(lsst::ctrl::events::StatusEvent) receiveStatusEvent(long timeout) {
        PTR(lsst::ctrl::events::Event) ev = self->receiveEvent(timeout);
        return boost::static_pointer_cast<lsst::ctrl::events::StatusEvent>(ev);
    }
    PTR(lsst::ctrl::events::CommandEvent) receiveCommandEvent() {
        PTR(lsst::ctrl::events::Event) ev = self->receiveEvent();
        return boost::static_pointer_cast<lsst::ctrl::events::CommandEvent>(ev);
    }
    PTR(lsst::ctrl::events::CommandEvent) receiveCommandEvent(long timeout) {
        PTR(lsst::ctrl::events::Event) ev = self->receiveEvent(timeout);
        return boost::static_pointer_cast<lsst::ctrl::events::CommandEvent>(ev);
    }
    PTR(lsst::ctrl::events::LogEvent) receiveLogEvent() {
        PTR(lsst::ctrl::events::Event) ev = self->receiveEvent();
        return boost::static_pointer_cast<lsst::ctrl::events::LogEvent>(ev);
    }
    PTR(lsst::ctrl::events::LogEvent) receiveLogEvent(long timeout) {
        PTR(lsst::ctrl::events::Event) ev = self->receiveEvent(timeout);
        return boost::static_pointer_cast<lsst::ctrl::events::LogEvent>(ev);
    }
}


/******************************************************************************/
// Local Variables: ***
// eval: (setq indent-tabs-mode nil) ***
// End: ***

