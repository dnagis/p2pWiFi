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

# Callbacks pour les Rx de signals
def deviceFound(devicepath):
	peer = bus.get_object("fi.w1.wpa_supplicant1", devicepath)
	peer_name = peer.Get('fi.w1.wpa_supplicant1.Peer', 'DeviceName', dbus_interface=dbus.PROPERTIES_IFACE)
	print("Device found: name = ", peer_name ," %s" % (devicepath))
	
def deviceLost(devicepath):
	print("Device lost: %s" % (devicepath))

#GONegotiationRequest ( o : path, q : dev_passwd_id, y : device_go_intent )	
#Selon https://docs.gtk.org/glib/gvariant-format-strings.html 
#	o --> object path
#	q --> guint16
#	y --> guchar
def gonegrequest(devicepath, pwd, intent):
	peer = bus.get_object("fi.w1.wpa_supplicant1", devicepath)
	peer_name = peer.Get('fi.w1.wpa_supplicant1.Peer', 'DeviceName', dbus_interface=dbus.PROPERTIES_IFACE)
	print("Device found: %s" % (devicepath), " name = ", peer_name)
	print("GO NEG REQUEST depuis: ", peer_name, " %s" % (devicepath))
	if peer_name == "XPS13" or peer_name == "NUC":
		print("Le peer est XPS13 ou NUC, on lance connect sur ", peer_name)
		#wpa_cli p2p_connect <BA_ADDR> pbc
		#connect: https://w1.fi/wpa_supplicant/devel/dbus.html fi.w1.wpa_supplicant1.Interface.P2PDevice Methods
		#entre XPS13 et NUC juste pour tester frequence j essaie 'frequency':2 j'ai resultat = fi.w1.wpa_supplicant1.ConnectChannelUnsupported: connect failed due to unsupported channel
		p2p_connect_arguments = dbus.Dictionary({'wps_method':'pbc','peer':peer,'persistent':True,'go_intent':15})
		p2p_interface.Connect(p2p_connect_arguments)


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

