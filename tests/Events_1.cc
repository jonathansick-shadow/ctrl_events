/**
 * @file Events_1.cc
 *
 * This tests the EventTransmitter class
 */

#include <iostream>
#include <stdexcept>
#include "lsst/ctrl/events.h"
#include "lsst/pex/exceptions.h"

namespace pexPolicy = lsst::pex::policy;
namespace pexExceptions = lsst::pex::exceptions;
namespace pexLogging = lsst::pex::logging;
namespace ctrlEvents = lsst::ctrl::events;

#define Assert(b, m) tattle(b, m, __LINE__)
    
void tattle(bool mustBeTrue, const string& failureMsg, int line) {
    if (! mustBeTrue) {
        ostringstream msg;
        msg << __FILE__ << ':' << line << ":\n" << failureMsg << ends;
        throw runtime_error(msg.str());
    }
}   
    
int main() {
    
    pexPolicy::Policy p;

    //
    // test EventTransmitter(const Policy& policy)
    //
    try {
        ctrlEvents::EventTransmitter et1(p);
    } catch (pexExceptions::NotFoundException&) { 
    } 

    p.set("topicName", "Events_1_test");
    p.set("useLocalSockets", false);
    try {
        ctrlEvents::EventTransmitter et2(p);
    } catch (pexExceptions::NotFoundException&) { 
    } 

    ctrlEvents::EventTransmitter et4("lsst8.ncsa.uiuc.edu", "Events_1_test");

    // test publish("string", PropertySet)
    PropertySet::Ptr psp1(new PropertySet);
    psp1->set("test2",12);
    et4.publish(psp1);

    PropertySet::Ptr psp2(new PropertySet);
    psp2->set("test3",(long)13);
    et4.publish(psp2);

    // test publish("string", LogRecord)
    pexLogging::LogRecord lr(-1, 10);
    const char *comment = "a comment";
    lr.addComment(comment);
    et4.publish(lr);

    // test getTopicName();
    std::string topicName = et4.getTopicName();
    Assert(topicName == "Events_1_test", "Topic name does not match initial name");
    std::cout << topicName << std::endl;
}
