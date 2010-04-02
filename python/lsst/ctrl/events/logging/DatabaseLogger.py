#!/usr/bin/env python

from lsst.cat.MySQLBase import MySQLBase
import os
import sys
import subprocess

class DatabaseLogger(MySQLBase):

    def __init__(self, dbHostName, portNumber):
        MySQLBase.__init__(self, dbHostName, portNumber)
        self.keywords = ['hostId', 'runId', 'sliceId', 'LEVEL', 'LOG', 'DATE', 'TIMESTAMP', 'COMMENT', 'STATUS', 'pipeline']
        self.keywordSet = set(self.keywords)

    def insertRecord(self, ps):

        hostId = ps.get("hostId")
        runId = ps.get("runId")
        sliceId = ps.get("sliceId")
        level = ps.get("LEVEL")
        log = ps.get("LOG")
        date = ps.get("DATE")
        ts = ps.get("TIMESTAMP")
        timestamp = ts.nsecs()

        commentList = ps.get("COMMENT")
        comment = ""
        
        if ps.valueCount("COMMENT") == 1:
            comment = commentList
        else:
            for i in commentList:
                if comment == "":
                    comment = i
                else:
                    comment = comment+";"+i


        if (ps.exists("STATUS")):
            status = ps.get("STATUS")
        else:
            status = "NULL"

        if (ps.exists("pipeline")):
            pipeline = ps.get("pipeline")
        else:
            pipeline = "NULL"


        names = ps.names()
        namesSet = set(names)

        diff = namesSet.difference(self.keywordSet)

        custom = ""
        for name in diff:
            if custom == "":
                custom = "%s : %s;" % (name,ps.get(name))
            else:
                custom = custom+ "%s : %s;" % (name,ps.get(name))
        if custom == "":
            custom = "NULL"

        cmd = """INSERT INTO logs.logger(hostId, runId, sliceid, status, level, log, date, TIMESTAMP, custom, comment, pipeline) values("%s", "%s", "%s", "%s", "%s", "%s", "%s", "%s", "%s", "%s", "%s")""" % (hostId, runId, sliceId, status, level, log, date, timestamp, custom, comment, pipeline)


        print cmd
        self.execCommand0(cmd)

