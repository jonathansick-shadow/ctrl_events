/**
 *
 *
 *
 */

#ifndef LSST_EVENTS_EXCEPTIONS_H
#define LSST_EVENTS_EXCEPTIONS_H

#include <stdexcept>

namespace lsst {
namespace events {

using namespace std;

class TopicNotSpecified : public runtime_error {
public:
    TopicNotSpecified() : runtime_error("Policy requires \"topicName\"") { }
};

class HostNotSpecified : public runtime_error {
public:
    HostNotSpecified() : runtime_error("Policy requires \"hostName\" if useLocalSockets is not set or is declared false") { }
};

class SocketFailed : public runtime_error {
public:
    SocketFailed() : runtime_error("local socket creation failed") { }
};

class CouldNotConnect : public runtime_error {
public:
    CouldNotConnect() : runtime_error("could not connect to receiver") { }
};

}} // end namespace lsst:events
#endif
