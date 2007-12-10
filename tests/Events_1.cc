/**
 * @file Events_1.cc
 *
 * This tests the EventTransmitter class
 */

#include <iostream>
#include <stdexcept>
#include "lsst/events/Events.h"
#include "lsst/mwi/exceptions.h"

using namespace std;
using lsst::mwi::policy::Policy;
using lsst::events::TopicNotSpecified;
using lsst::events::HostNotSpecified;
using lsst::events::CouldNotConnect;
using lsst::mwi::exceptions::NotFound;

using lsst::events::EventTransmitter;

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

    //
    // test EventTransmitter(const Policy& policy)
    //
    try {
        EventTransmitter et1(p);
    } catch (NotFound&) { 
    } 

    p.set("topicName", "Events_1_test");
    p.set("useLocalSockets", false);
    try {
        EventTransmitter et2(p);
    } catch (NotFound&) { 
    } 

    p.set("useLocalSockets", true);
    try {
        EventTransmitter et3(p);
    } catch (CouldNotConnect&) { 
    } 

    EventTransmitter et4("lsst8.ncsa.uiuc.edu", "Events_1_test");

    // test publish("string", DataProperty)
    DataProperty dp("test", 12);

    DataProperty::PtrType dpt(new DataProperty("test",12));
    et4.publish(std::string("test log"), dpt);

    // test publish("string", LogRecord)
    LogRecord lr(-1, 10);
    const char *comment = "a comment";
    lr.addComment(comment);
    et4.publish("test log", lr);

    // test getTopicName();
    std::string topicName = et4.getTopicName();
    Assert(topicName == "Events_1_test", "Topic name does not match initial name");
    std::cout << topicName << std::endl;
}
