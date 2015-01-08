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
    _IPAddress = host.getIPAddress() & 0x0FFFFFFFF;
    _pid = getpid();
    _localID = _localCounter++;
}

/** \brief LocationID object. This object represents the originating process
  *        of an event.  When created, this represents a reconstituted ID.
  */
LocationID::LocationID(int ipAddress, int pid, int localID) {
    _constructor(ipAddress, pid, localID);
}

LocationID::LocationID(const LocationID& id) {
    _constructor(id.getIPAddress(), id.getProcessID(), id.getLocalID());
}

void LocationID::_constructor(int ipAddress, int pid, int localID) {
    _IPAddress = ipAddress;
    _pid = pid;
    _localID = localID;
}

/** \brief destructor
  */
LocationID::~LocationID() {
}

int LocationID::_localCounter=0;
int LocationID::_localID=0;
int LocationID::_IPAddress=0;
int LocationID::_pid=0;

int LocationID::getIPAddress() const {
    return _IPAddress;
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
