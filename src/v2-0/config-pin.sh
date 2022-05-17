#!/bin/bash
echo stop > /sys/class/remoteproc/remoteproc1/state
echo stop > /sys/class/remoteproc/remoteproc2/state

echo pru0.out > /sys/class/remoteproc/remoteproc1/firmware
echo pru1.out > /sys/class/remoteproc/remoteproc2/firmware

echo start > /sys/class/remoteproc/remoteproc1/state
echo start > /sys/class/remoteproc/remoteproc2/state

config-pin p8_16 pruin
config-pin p8_28 pruin
config-pin p8_11 pruout
config-pin p8_27 pruout
