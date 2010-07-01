#!/usr/bin/env python

from lsst.cat.MySQLBase import MySQLBase
import os
import sys
import subprocess

class DatabaseLogger(MySQLBase):

    def __init__(self, dbHostName, portNumber):
        MySQLBase.__init__(self, dbHostName, portNumber)
        self.keywords = ['HOSTID', 'RUNID', 'SLICEID', 'LEVEL', 'LOG', 'DATE', 'NODE', 'TIMESTAMP', 'COMMENT', 'STATUS', 'PIPELINE', 'pipeline', 'EVENTTIME', 'PUBTIME', 'TYPE', 'STAGEID', 'stageId', 'LOOPNUM', 'loopnum']
        self.keywordSet = set(self.keywords)

    def insertRecord(self, dbTable, ps):
        #runId = ps.get("RUNID")
        #dbName = "logs.%s" % runId
        #if self.dbExists(dbName) == False:
        #    self.createDb(self,dbName)
        #self.insertRecord(ps, dbName)
        self._insertRecord(dbTable, ps)

    def _insertRecord(self, dbTable, ps):

        hostId = ps.get("HOSTID")
        runId = ps.get("RUNID")
        sliceId = ps.get("SLICEID")
        level = ps.get("LEVEL")
        log = ps.get("LOG")
        date = ps.get("DATE")
        ts = ps.get("TIMESTAMP")
        eventtime = ps.get("EVENTTIME")
        pubtime = ps.get("PUBTIME")
        eventtype = ps.get("TYPE")
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

        status = "NULL"
        if ps.exists("STATUS"):
            status = ps.get("STATUS")

        if ps.exists("pipeline"):
            pipeline = ps.get("pipeline")
        elif ps.exists("PIPELINE"):
            pipeline = ps.get("PIPELINE")
        else:
            pipeline = "NULL"

        stageid = "-1"
        if ps.exists("stageId"):
            stageid = ps.get("stageId")
        elif ps.exists("STAGEID"):
            stageid = ps.get("STAGEID")

        loopnum = "-1"
        if ps.exists("loopnum"):
            loopnum = ps.get("loopnum")
        elif ps.exists("LOOPNUM"):
            loopnum = ps.get("LOOPNUM")



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
        custom = custom[0:4096]
        comment = comment[0:2048]

        cmd = """INSERT INTO logs.%s(hostId, runId, sliceid, status, level, log, date, node, TIMESTAMP, custom, comment, pipeline, eventtime, pubtime, type, stageid, loopnum) values("%s", "%s", "%s", "%s", "%s", "%s", "%s", "%s", "%s", "%s", "%s", "%s", "%s", "%s", "%s", "%s", "%s")""" % (dbTable, hostId, runId, sliceId, status, level, log, date, node, timestamp, custom, comment, pipeline, eventtime, pubtime, eventtype, stageid, loopnum)


        self.execCommand0(cmd)

