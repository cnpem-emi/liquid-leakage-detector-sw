#!/bin/sh

export SLOTS=/sys/devices/bone_capemgr.9/slots
export PINS=/sys/kernel/debug/pinctrl/44e10800.pinmux/pins

echo "Compilando Device Tree Overlay..."

dtc -O dtb -o Reflexao-00A0.dtbo -b 0 -@ Reflexao-00A0.dts

cp Reflexao-00A0.dtbo /lib/firmware

rm Reflexao-00A0.dtbo

