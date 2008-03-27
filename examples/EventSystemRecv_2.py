import lsst.events as events
import lsst.daf.data as datap

if __name__ == "__main__":

    system = events.EventSystem.getDefaultEventSystem()
    system.createReceiver("lsst8.ncsa.uiuc.edu", "test1")
    system.createReceiver("lsst8.ncsa.uiuc.edu", "test2")
    system.createReceiver("lsst8.ncsa.uiuc.edu", "test3")

    dp1 = system.receive("test1", 5000)
    if dp1.get() == None:
        print "Nothing received from test1"
    else:
        print dp1.toString(" ",1)


    dp2 = system.receive("test2", 5000)
    if dp2.get() == None:
        print "Nothing received from test2"
    else:
        print dp2.toString(" ",1)

    dp3 = system.receive("test3", 5000)
    if dp3.get() == None:
        print "Nothing received from test3"
    else:
        print dp3.toString(" ",1)

