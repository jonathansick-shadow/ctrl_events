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
using lsst::pex::policy::Policy;
using lsst::pex::exceptions::NotFoundException;
using lsst::pex::exceptions::RuntimeErrorException;

// using namespace lsst::ctrl::events::EventSystem;
using namespace lsst::ctrl::events;

#define Assert(b, m) tattle(b, m, __LINE__)
    
void tattle(bool mustBeTrue, const string& failureMsg, int line) {
    if (! mustBeTrue) {
        ostringstream msg;
        msg << __FILE__ << ':' << line << ":\n" << failureMsg << ends;
        throw LSST_EXCEPT(RuntimeErrorException, msg.str());
    }
}   
    
int main() {
    
    Policy p;

    EventSystem eventSystem = EventSystem().getDefaultEventSystem();
    //
    // test EventTransmitter(const Policy& policy)
    //
    try {
        eventSystem.createTransmitter(p);
    } catch (NotFoundException&) { 
    } 

    p.set("topicName", "EventSystem_1_test");
    p.set("useLocalSockets", false);
    try {
        eventSystem.createTransmitter(p);
    } catch (NotFoundException&) { 
    } 

    p.set("useLocalSockets", true);
    try {
        eventSystem.createTransmitter(p);
    } catch (RuntimeErrorException&) { 
    } 

    eventSystem.createTransmitter("lsst8.ncsa.uiuc.edu", "EventSystem_1_test");

    // test publish("string", DataProperty)
    PropertySet::Ptr psp(new PropertySet);
    psp->add("test", 12);

    eventSystem.publish("EventSystem_1_test", psp);

    // test publish("string", LogRecord)
    LogRecord lr(-1, 10);
    const char *comment = "a comment";
    lr.addComment(comment);
    eventSystem.publish("EventSystem_1_test", lr);

}
