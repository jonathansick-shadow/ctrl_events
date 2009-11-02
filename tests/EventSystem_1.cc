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
    try {
        eventSystem.createTransmitter(p);
    } catch (pexExceptions::NotFoundException&) { 
    } 

    eventSystem.createTransmitter("lsst8.ncsa.uiuc.edu", "EventSystem_1_test");

    // test publish("string", DataProperty)
    PropertySet::Ptr psp(new PropertySet);
    psp->add("test", 12);

    eventSystem.publish("EventSystem_1_test", psp);

    // test publish("string", LogRecord)
    pexLogging::LogRecord lr(-1, 10);
    const char *comment = "a comment";
    lr.addComment(comment);
    eventSystem.publish("EventSystem_1_test", lr);

}
