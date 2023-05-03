#!/usr/bin/env python3

#Accès à wpa_supplicant via dbus-python. 
#Lance un find, attends signal go-neg-request, lance un connect 



import dbus
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib
import signal

def sigint_handler(sig, frame):
    if sig == signal.SIGINT:
        loop.quit()
    else:
        raise ValueError("Undefined handler for '{}'".format(sig))

# Callbacks pour les Rx de Signals
def deviceFound(devicepath):
	print("Device found: %s" % (devicepath))
	
def deviceLost(devicepath):
	print("Device lost: %s" % (devicepath))

#GONegotiationRequest ( o : path, q : dev_passwd_id, y : device_go_intent )	
def gonegrequest(devicepath, pwd, intent):
	print("GO NEG REQUEST depuis: %s" % (devicepath))


if __name__ == '__main__':
	
	DBusGMainLoop(set_as_default=True)
	
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
	
	#enregistrement des callbacks pour les signaux dbus
	bus.add_signal_receiver(deviceFound,dbus_interface=wpas_dbus_interfaces_p2pdevice,signal_name="DeviceFound")
	bus.add_signal_receiver(deviceLost,dbus_interface=wpas_dbus_interfaces_p2pdevice,signal_name="DeviceLost")
	bus.add_signal_receiver(gonegrequest,dbus_interface=wpas_dbus_interfaces_p2pdevice,signal_name="GONegotiationRequest")
	
	#P2PFindDict = dbus.Dictionary({'Timeout':int(600)}) #Après Timeout s: P2P-FIND-STOPPED
	#Timeout: README-P2P: "Timeout - Optional ASCII base-10-encoded u16. If missing, request will not time out and must be canceled manually
	P2PFindDict = dbus.Dictionary() #Aucun des arguments n'est mandatory 
	p2p_interface.Find(P2PFindDict)
	
	
	#On lance un loop 
	signal.signal(signal.SIGINT, sigint_handler)
	loop = GLib.MainLoop()
	loop.run()

