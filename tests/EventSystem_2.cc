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
    try {
        eventSystem.createReceiver(p);
    } catch (NotFoundException&) { 
    } 

    eventSystem.createReceiver("lsst8.ncsa.uiuc.edu", "EventSystem_2_test");

}
