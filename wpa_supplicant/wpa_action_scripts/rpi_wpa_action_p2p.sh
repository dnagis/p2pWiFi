#!/bin/sh

#wpa action script du RPi sur p2p-dev-wlan0 (default iface) lancé depuis /bin/wifi --> wpa_cli -a /share/wpa_action_scripts/rpi_wpa_action_p2p.sh -B

logger "script /share/wpa_action_scripts/rpi_wpa_action_p2p.sh les variables: " $1 $2 $3 $4

if [ "$2" = "P2P-GROUP-STARTED" ]; then
	logger "rpi_wpa_action_p2p.sh: Rx: P2P-GROUP-STARTED on assigne l'IP 192.168.49.1 à l'interface $3"
	ifconfig $3 192.168.49.1
fi
