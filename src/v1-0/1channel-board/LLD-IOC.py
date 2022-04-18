#!/usr/bin/python
# -*- coding: utf-8 -*-

# ----- Liquid Leak Detect - IOC -----
# IOC for LLD hardware, which will detect liquid leaks (water) along the booster and storage ring.
# Works based on principles of signal reflexion in an unmatched transmission line.
# Hardware: Carlos Scorzato

# Based on SI-MBTemp (Eduardo Pereira Coelho).

# Author: Patricia HENRIQUES NALLIN


from pcaspy import Driver, Alarm, Severity, SimpleServer
from Queue import PriorityQueue
import sys, time
import threading
import Reflexao

# Number of sectors/sensors monitored by this network
NSECTORS = 7

# Max range for leak detection [m]
DEVIATION = 1

# Time between monitoring sessions [sec]
MONITOR_TIME = 5

PVs = {}

for Berco in range(1, NSECTORS + 1):
    PVs["LeakMonitor:Section" + "{0:02d}".format(Berco) + "-Pos"] = {"type": "float", "prec": 2, "unit": "m"}

PVs["LeakMonitor:CurrentLeak-Pos"] = {"type": "float", "prec": 2, "unit": "m"}
PVs["LeakMonitor:Mode"] = {"type": "enum", "enums": ["MONITOR", "CALIBRATE"]}


class SV_Driver(Driver):
    def __init__(self):

        Driver.__init__(self)
        Reflexao.Init()

	# PVs
        for Berco in range(1, NSECTORS + 1):
            self.setParam("LeakMonitor:Section" + "{0:02d}".format(Berco) + "-Pos", Berco - 1 + 0.5)
        for Berco in range(1, NSECTORS + 1):
            self.setParamStatus("LeakMonitor:Section" + "{0:02d}".format(Berco) + "-Pos", Alarm.NO_ALARM,
                                Severity.NO_ALARM)
	self.setParamStatus("LeakMonitor:CurrentLeak-Pos", Alarm.NO_ALARM, Severity.NO_ALARM)
	self.setParamStatus("LeakMonitor:Mode", Alarm.NO_ALARM, Severity.NO_ALARM)


        # Priority Queue
        self.queue = PriorityQueue()

        # Threads
        self.process = threading.Thread(target=self.processThread)
        self.scan = threading.Thread(target=self.scanThread)

        self.process.setDaemon(True)
        self.scan.setDaemon(True)

        self.process.start()
        self.scan.start()

    # Monitoring - Requests position of the leak
    def scanThread(self):
        while (True):
            if self.getParam("LeakMonitor:Mode") == 0:
                self.queue.put((1, ["MONITOR"]))
            time.sleep(MONITOR_TIME)

    def processThread(self):
        while (True):
            # Gets next operation
            item = self.queue.get(block=True)
            item = item[1]

            # Monitoring: pulses and stores the position of the leak
            if (item[0] == "MONITOR"):
                CurrentLeak = Reflexao.Pulsar()
                self.setParam("LeakMonitor:CurrentLeak-Pos", CurrentLeak)

                # Verify if leak is related to a section
                for i in range(NSECTORS):
                    if abs(CurrentLeak - self.getParam(
                                            "LeakMonitor:Section" + "{0:02d}".format(i + 1) + "-Pos")) < DEVIATION:
                        print "Leak detected on SECTION " + str(i + 1) + "!!!"
                        
                self.updatePVs()


            # Calibrate relative positions of the sector
            elif (item[0] == "CALIBRATE"):
                self.setParam("LeakMonitor:Section" + "{0:02d}".format(item[1]) + "-Pos", Reflexao.Pulsar())
                self.updatePVs()


    def write(self, reason, value):
        
        # Operation Mode
        if reason == "LeakMonitor:Mode":
            self.setParam(reason, value)
            self.updatePVs()
            return (True)

        # If CALIBRATE mode, select SECTOR which will be calibrated writing 0 to its value
        elif self.getParam("LeakMonitor:Mode") == 1:
            if value == 0:
                self.queue.put((1, ["CALIBRATE", int(reason[-6:-4])]))
                return (True)

	# Position: not writtable by the user
        elif reason[-4:] == "-Pos":
            return (False)


if (__name__ == '__main__'):

    CAserver = SimpleServer()
    CAserver.createPV("", PVs)
    driver = SV_Driver()

    while (True):
        CAserver.process(0.1)

