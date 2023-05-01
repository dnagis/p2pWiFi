#!/bin/bash

#wpa action script en focus sur p2p-dev-wlan0 (voir sysinit)

logger "script /usr/share/wpa_action_scripts/wpa_action_p2p.sh (core.sfs) les variables: " $1 $2 $3 $4

if [ "$2" = "P2P-GROUP-STARTED" ]; then
	logger "wpa_action_p2p.sh: Rx: P2P-GROUP-STARTED on assigne l'IP 192.168.49.2 à l'interface $3"
	ifconfig $3 192.168.49.2
fi
