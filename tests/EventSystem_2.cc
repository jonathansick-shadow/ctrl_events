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
 * @file EventSystem_2.cc
 *
 * This tests the EventSystem class
 */
#include <iostream>
#include <stdexcept>
#include "lsst/ctrl/events/EventSystem.h"
#include "lsst/pex/exceptions.h"

using namespace std;
using lsst::pex::policy::Policy;
using lsst::pex::exceptions::NotFoundException;
using lsst::pex::exceptions::RuntimeErrorException;

using lsst::ctrl::events::EventSystem;

#define Assert(b, m) tattle(b, m, __LINE__)
    
void tattle(bool mustBeTrue, const string& failureMsg, int line) {
    if (! mustBeTrue) {
        ostringstream msg;
        msg << __FILE__ << ':' << line << ":\n" << failureMsg << ends;
        throw runtime_error(msg.str());
    }
}   
    
int main() {
    
    Policy p;

    EventSystem eventSystem = EventSystem().getDefaultEventSystem();
    //
    // test EventTransmitter(const Policy& policy)
    //
    try {
        eventSystem.createReceiver(p);
    } catch (NotFoundException&) { 
    } 

    p.set("topicName", "EventSystem_2_test");
    p.set("useLocalSockets", false);
    try {
        eventSystem.createReceiver(p);
    } catch (NotFoundException&) { 
    } 

    p.set("topicName", "EventSystem_2_test");
    p.set("useLocalSockets", false);
    p.set("hostName", "garbage");
    try {
        eventSystem.createReceiver(p);
    } catch (RuntimeErrorException&) { 
    } 

    try {
        eventSystem.createReceiver("lsst8.ncsa.uiuc.edu", "EventSystem_2_test");
    } catch (RuntimeErrorException&) { 
        // can't create a receiver if one already exists for that topic
    } 
    eventSystem.createReceiver("lsst8.ncsa.uiuc.edu", "EventSystem_2_test2");
}
