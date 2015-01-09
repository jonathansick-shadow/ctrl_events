#include <sys/types.h>
#include <unistd.h>
#include "lsst/ctrl/events/LocationID.h"
#include "lsst/ctrl/events/Host.h"

namespace lsst {
namespace ctrl {
namespace events {

/** \brief LocationID object. This object represents the originating process
  *        of an event.  When created, this represents a unique ID.
  */
LocationID::LocationID() {
    Host host = Host().getHost();
    _hostname = host.getHostName();
    _pid = getpid();
    _localID = _localCounter++;
}

/** \brief LocationID object. This object represents the originating process
  *        of an event.  When created, this represents a reconstituted ID.
  */
LocationID::LocationID(const std::string& hostname, int pid, int localID) {
    _constructor(hostname, pid, localID);
}

LocationID::LocationID(const LocationID& id) {
    _constructor(id.getHostName(), id.getProcessID(), id.getLocalID());
}

void LocationID::_constructor(const std::string& hostname, int pid, int localID) {
    _hostname = hostname;
    _pid = pid;
    _localID = localID;
}

/** \brief destructor
  */
LocationID::~LocationID() {
}

int LocationID::_localCounter=0;
int LocationID::_localID=0;
int LocationID::_pid=0;
std::string LocationID::_hostname;

std::string LocationID::getHostName() const {
    return _hostname;
}

int LocationID::getProcessID() const {
    return _pid;
}

int LocationID::getLocalID() const {
    return _localID;
}

}
}
}
