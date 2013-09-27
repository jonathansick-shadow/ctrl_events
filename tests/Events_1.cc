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
    
    std::ostringstream oss;
    pexPolicy::Policy p;
    char host[128];

    int ret = gethostname(host,sizeof(host));
    if (ret != 0)
        throw runtime_error("error getting hostname");
    oss << "events_1_test_" << host << "_" << getpid();
    std::string topic = oss.str();
    

    //
    // test EventTransmitter(const Policy& policy)
    //
    try {
        ctrlEvents::EventTransmitter et1(p);
    } catch (pexExceptions::NotFoundException&) { 
    } 

    p.set("topicName", topic);
    p.set("useLocalSockets", false);
    try {
        ctrlEvents::EventTransmitter et2(p);
    } catch (pexExceptions::NotFoundException&) { 
    } 

    p.set("topicName", topic);
    p.set("useLocalSockets", false);
    p.set("hostName", "garbage");
    try {
        ctrlEvents::EventTransmitter et2(p);
    } catch (pexExceptions::RuntimeErrorException&) { 
    } 

    ctrlEvents::EventTransmitter et4("lsst8.ncsa.illinois.edu", topic);

    // test publish("string", PropertySet)
    PropertySet::Ptr psp1(new PropertySet);
    psp1->set("test2",12);

    ctrlEvents::Event event1("event1", psp1);
    et4.publishEvent(event1);

    PropertySet::Ptr psp2(new PropertySet);
    psp2->set("test3",(long)13);
    ctrlEvents::Event event2("event2", psp2);
    et4.publishEvent(event2);

    // test publish("string", LogRecord)
    pexLogging::LogRecord lr(-1, 10);
    const char *comment = "a comment";
    lr.addComment(comment);
    ctrlEvents::LogEvent logEvent("logrec", lr);
    et4.publishEvent(logEvent);

    // test getTopicName();
    std::string topicName = et4.getTopicName();
    Assert(topicName == topic, "Topic name does not match initial name");
    std::cout << topicName << std::endl;

    // test publish("string", LogRecord)
    pexLogging::LogRecord lr2(-1, 10);
    const char *comment2 = "a comment";
    lr2.addComment(comment2);
    lr2.addComment("a second comment");
    lr2.addProperty("LOG", "Log value");
    lr2.addProperty("NAME", "name value");

    ctrlEvents::LogEvent logEvent2("myrunid", lr2);
    et4.publishEvent(logEvent2);

    std::vector<std::string>vec = logEvent.getComment();
    std::vector<std::string>::iterator iter;
    for (iter = vec.begin(); iter != vec.end(); iter++) {
        std::cout << *iter << std::endl;
    }
}
