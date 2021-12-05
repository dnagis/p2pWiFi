#!/usr/bin/env python3

#accès à wpa_supplicant via dbus-python
#wpa_supplicant-2.9/wpa_supplicant/examples/p2p/p2p_find.py
#mais avec un loop qui ne laisse mes CPU tranquilles (voir fichier à côté: simple_loop.py)


import dbus
from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib
import signal

def sigint_handler(sig, frame):
    if sig == signal.SIGINT:
        loop.quit()
    else:
        raise ValueError("Undefined handler for '{}'".format(sig))

# Required Signals
def deviceFound(devicepath):
	print("Device found: %s" % (devicepath))
	addr=devicepath.DeviceAddress
	print("addr: %s"+addr)

def deviceLost(devicepath):
	print("Device lost: %s" % (devicepath))


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
	print("path=" + path)
	
	interface_object = bus.get_object(wpas_dbus_interface, path)
	p2p_interface = dbus.Interface(interface_object,wpas_dbus_interfaces_p2pdevice)
	
	bus.add_signal_receiver(deviceFound,dbus_interface=wpas_dbus_interfaces_p2pdevice,signal_name="DeviceFound")
	bus.add_signal_receiver(deviceLost,dbus_interface=wpas_dbus_interfaces_p2pdevice,signal_name="DeviceLost")
	
	P2PFindDict = dbus.Dictionary({'Timeout':int(60)})
	p2p_interface.Find(P2PFindDict)
	
	
	signal.signal(signal.SIGINT, sigint_handler)
	loop = GLib.MainLoop()
	loop.run()
