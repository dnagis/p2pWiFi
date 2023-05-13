#!/usr/bin/env python3

#p2p_find.py avec ajout: se connecte si device found 
#pour Zero: p2p_connect.py Zero


import dbus
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib
import signal
import sys




def sigint_handler(sig, frame):
    if sig == signal.SIGINT:
        loop.quit()
    else:
        raise ValueError("Undefined handler for '{}'".format(sig))


#Callback device found, si DeviceName = sys.argv[1] --> Connect()
def deviceFound(devicepath):
	print("Device found: %s" % (devicepath))
	peer_found = bus.get_object("fi.w1.wpa_supplicant1", devicepath)
	peer_macaddr = peer_found.Get('fi.w1.wpa_supplicant1.Peer', 'DeviceAddress', dbus_interface=dbus.PROPERTIES_IFACE)
	peer_name = peer_found.Get('fi.w1.wpa_supplicant1.Peer', 'DeviceName', dbus_interface=dbus.PROPERTIES_IFACE)
	print("DeviceName = ", peer_name, " dev_searched = ", dev_searched)

	
	if(peer_name == dev_searched):
		print("\033[1;32mDevice %s found on tente connection\033[0;39m" % dev_searched)
		#wpa_cli p2p_connect <BA_ADDR> pbc join
		#connect: https://w1.fi/wpa_supplicant/devel/dbus.html fi.w1.wpa_supplicant1.Interface.P2PDevice Methods
		#p2p_connect_arguments = dbus.Dictionary({'wps_method':'pbc','peer':peer_found,'join':True})
		p2p_connect_arguments = dbus.Dictionary({'wps_method':'pbc','peer':peer_found,'frequency':2412,'go_intent':0})
		p2p_interface.Connect(p2p_connect_arguments)

def groupStarted(properties):
	print("\033[1;32msignal dbus GroupStarted\033[0;39m")
	#loop.quit()


if __name__ == '__main__':
	
	DBusGMainLoop(set_as_default=True)
	
	dev_searched = sys.argv[1]
	wpas_dbus_interface = "fi.w1.wpa_supplicant1"
	interface_name = "wlan0"
	wpas_dbus_opath = "/fi/w1/wpa_supplicant1"
	wpas_wpas_dbus_interfaces_opath = "/fi/w1/wpa_supplicant1/Interfaces"
	wpas_dbus_interfaces_interface = wpas_dbus_interface + ".Interface"
	wpas_dbus_interfaces_p2pdevice = wpas_dbus_interfaces_interface + ".P2PDevice"
	
	bus = dbus.SystemBus()
	wpas_object = bus.get_object(wpas_dbus_interface,wpas_dbus_opath)
	wpas = dbus.Interface(wpas_object,wpas_dbus_interface)
	
	path = wpas.GetInterface(interface_name)
	#print("path=" + path)
	
	interface_object = bus.get_object(wpas_dbus_interface, path)
	p2p_interface = dbus.Interface(interface_object,wpas_dbus_interfaces_p2pdevice)
	
	#enregistrement callback pour les signaux dbus
	bus.add_signal_receiver(deviceFound,dbus_interface=wpas_dbus_interfaces_p2pdevice,signal_name="DeviceFound")
	bus.add_signal_receiver(groupStarted,dbus_interface=wpas_dbus_interfaces_p2pdevice,signal_name="GroupStarted")
	
	#Jai mis 30 pendant longtemps, et j'attendais souvent plus que 30s. ***sigh*** Pourtant n'est pas mandatory
	#P2PFindDict = dbus.Dictionary({'Timeout':int(600)})
	P2PFindDict = dbus.Dictionary({})
	p2p_interface.Find(P2PFindDict)
	print("P2P Find lancé, on cherche: ", dev_searched)
	
	#On lance un loop 
	signal.signal(signal.SIGINT, sigint_handler)
	loop = GLib.MainLoop()
	loop.run()

