#!/usr/bin/env python3


#Hello_wpas
#Bricoler avec l'api dbus de wpa_supplicant fi.w1.wpa_supplicant1 (https://w1.fi/wpa_supplicant/devel/dbus.html)

#en utilisant les recettes dbus-python
#https://dbus.freedesktop.org/doc/dbus-python/tutorial.html

#Ici le plus simple: appeler la methode GetInterface de fi.w1.wpa_supplicant1



import dbus

bus = dbus.SystemBus()

proxy = bus.get_object('fi.w1.wpa_supplicant1','/fi/w1/wpa_supplicant1') # proxy is a dbus.proxies.ProxyObject

#je veux appeler la method GetInterface ( s : ifname ) –> o : interface "Returns a D-Bus path to an object related to an interface which wpa_supplicant already controls."
# Arguments s : ifname Name of the network interface, e.g., wlan0
# Returns o : interface A D-Bus path to an object representing an interface

path = proxy.GetInterface('wlan0', dbus_interface='fi.w1.wpa_supplicant1') #si dbus_interface= en premier SyntaxError: positional argument follows keyword argument
print(path)










