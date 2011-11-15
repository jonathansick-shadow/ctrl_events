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
 
/**
 * @file EventSystem_1.cc
 *
 * This tests the EventSystem class
 */
#include <iostream>
#include <stdexcept>
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/pex/exceptions.h"

using namespace std;

namespace pexExceptions = lsst::pex::exceptions;
namespace pexLogging = lsst::pex::logging;
namespace pexPolicy = lsst::pex::policy;
namespace ctrlEvents = lsst::ctrl::events;

#define Assert(b, m) tattle(b, m, __LINE__)
    
void tattle(bool mustBeTrue, const string& failureMsg, int line) {
    if (! mustBeTrue) {
        ostringstream msg;
        msg << __FILE__ << ':' << line << ":\n" << failureMsg << ends;
        throw LSST_EXCEPT(pexExceptions::RuntimeErrorException, msg.str());
    }
}   
    
int main() {
    
    pexPolicy::Policy p;

    ctrlEvents::EventSystem eventSystem = ctrlEvents::EventSystem().getDefaultEventSystem();
    //
    // test EventTransmitter(const Policy& policy)
    //
    try {
        eventSystem.createTransmitter(p);
    } catch (pexExceptions::NotFoundException&) { 
    } 

    p.set("topicName", "EventSystem_1_test");
    p.set("useLocalSockets", false);
    try {
        eventSystem.createTransmitter(p);
    } catch (pexExceptions::NotFoundException&) { 
        std::cout << "not created" << std::endl;
    } 

    eventSystem.createTransmitter("lsst8.ncsa.uiuc.edu", "EventSystem_1_test");

    // test publish("string", DataProperty)
    PropertySet::Ptr psp(new PropertySet);
    psp->add("test", 12);

    ctrlEvents::Event event("runid1",psp);
    eventSystem.publishEvent("EventSystem_1_test", event);

    // test publish("string", LogRecord)
    pexLogging::LogRecord lr(-1, 10);
    const char *comment = "a comment";
    lr.addComment(comment);
    ctrlEvents::LogEvent logEvent("logrec", lr);
    eventSystem.publishEvent("EventSystem_1_test", logEvent);

}
