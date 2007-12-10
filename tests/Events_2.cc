/**
 * @file Events_2.cc
 *
 * This tests the EventReceiver class
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

using lsst::events::EventReceiver;

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
    // test EventReceiver(const Policy& policy)
    //
    try {
        EventReceiver er1(p);
    } catch (NotFound&) { 
    } 

    p.set("topicName", "Events_1_test");
    p.set("useLocalSockets", false);
    try {
        EventReceiver er2(p);
    } catch (HostNotSpecified&) { 
    } 


    EventReceiver er3("lsst8.ncsa.uiuc.edu", "Events_2_test");

    // test getTopicName();
    std::string topicName = er3.getTopicName();
    Assert(topicName == "Events_2_test", "Topic name does not match initial name");
}
