#!/usr/bin/env python

from lsst.cat.MySQLBase import MySQLBase
import os
import sys
import subprocess

class DatabaseLogger(MySQLBase):

    def __init__(self, dbHostName, portNumber):
        MySQLBase.__init__(self, dbHostName, portNumber)
        self.keywords = ['HOSTID', 'RUNID', 'SLICEID', 'LEVEL', 'LOG', 'DATE', 'NODE', 'TIMESTAMP', 'COMMENT', 'STATUS', 'PIPELINE']
        self.keywordSet = set(self.keywords)

    def insertRecord(self, ps):

        hostId = ps.get("HOSTID")
        runId = ps.get("RUNID")
        sliceId = ps.get("SLICEID")
        level = ps.get("LEVEL")
        log = ps.get("LOG")
        date = ps.get("DATE")
        ts = ps.get("TIMESTAMP")
        node = -1
        if ps.exists("NODE"):
            node = ps.get("NODE")

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


        if (ps.exists("TOPIC")):
            ps.remove("TOPIC")
        if (ps.exists("STATUS")):
            status = ps.get("STATUS")
        else:
            status = "NULL"

        if (ps.exists("PIPELINE")):
            pipeline = ps.get("PIPELINE")
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

        cmd = """INSERT INTO logs.logger(hostId, runId, sliceid, status, level, log, date, node, TIMESTAMP, custom, comment, pipeline) values("%s", "%s", "%s", "%s", "%s", "%s", "%s", "%s", "%s", "%s", "%s", "%s")""" % (hostId, runId, sliceId, status, level, log, date, node, timestamp, custom, comment, pipeline)


        print cmd
        self.execCommand0(cmd)

