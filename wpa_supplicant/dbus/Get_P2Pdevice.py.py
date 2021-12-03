#!/usr/bin/env python3



import sys
import dbus

bus = dbus.SystemBus()
wpa_sup_obj = bus.get_object('fi.w1.wpa_supplicant1', '/fi/w1/wpa_supplicant1')
props_iface = dbus.Interface(wpa_sup_obj, "org.freedesktop.DBus.Properties")
interfaces = props_iface.Get('fi.w1.wpa_supplicant1', "Interfaces")

interface = interfaces[0]

print("Je vois: %s" % (interface))

interface_obj = bus.get_object('fi.w1.wpa_supplicant1', interface)
interface_interface_props = dbus.Interface(interface_obj, "org.freedesktop.DBus.Properties")
interface_interface = dbus.Interface(interface_obj, "fi.w1.wpa_supplicant1.Interface.P2PDevice")


print("Interface's name is %s" % adapters_name)

