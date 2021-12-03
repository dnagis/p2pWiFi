#!/usr/bin/env python3

#https://dbus.freedesktop.org/doc/dbus-python/tutorial.html
#https://w1.fi/wpa_supplicant/devel/dbus.html#dbus_p2pdevice

import dbus
bus = dbus.SystemBus()
proxy = bus.get_object('fi.w1.wpa_supplicant1',
                       '/fi/w1/wpa_supplicant1/Interface/P2PDevice')
