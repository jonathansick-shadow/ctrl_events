/**
 * @file EventSystem_2.cc
 *
 * This tests the EventSystem class
 */
#include <iostream>
#include <stdexcept>
#include "lsst/events/EventSystem.h"
#include "lsst/mwi/exceptions.h"
using namespace std;
using lsst::mwi::policy::Policy;
using lsst::events::TopicNotSpecified;
using lsst::events::HostNotSpecified;
using lsst::events::CouldNotConnect;
using lsst::mwi::exceptions::NotFound;

using lsst::events::EventSystem;

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
    } catch (NotFound&) { 
    } 

    p.set("topicName", "EventSystem_2_test");
    p.set("useLocalSockets", false);
    try {
        eventSystem.createReceiver(p);
    } catch (HostNotSpecified&) { 
    } 

    p.set("useLocalSockets", true);
    try {
        eventSystem.createTransmitter(p);
    } catch (CouldNotConnect&) { 
    } 

    eventSystem.createReceiver("lsst8.ncsa.uiuc.edu", "EventSystem_2_test");

}
