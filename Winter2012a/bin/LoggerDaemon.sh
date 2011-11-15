#!/bin/sh
name=~/Logger_`date "+%m%d%y"`.txt
nohup Logger3.py lsst8.ncsa.uiuc.edu lsst10.ncsa.uiuc.edu 3306 2>&1 >$name &
