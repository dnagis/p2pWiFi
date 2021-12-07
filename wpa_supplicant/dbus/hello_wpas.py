#!/usr/bin/env python3


#Hello_wpas

#Appel le plus simple possible de wpa_supplicant
#en utilisant dbus-python: https://dbus.freedesktop.org/doc/dbus-python/tutorial.html

#api dbus de wpa_supplicant fi.w1.wpa_supplicant1 (https://w1.fi/wpa_supplicant/devel/dbus.html)



#appel de la methode GetInterface() définie dans fi.w1.wpa_supplicant1 (https://w1.fi/wpa_supplicant/devel/dbus.html)



import dbus



bus = dbus.SystemBus()

proxy = bus.get_object('fi.w1.wpa_supplicant1','/fi/w1/wpa_supplicant1') # proxy est un proxy vers wpas: de type: dbus.proxies.ProxyObject

#je veux appeler la method GetInterface ( s : ifname ) –> o : interface "Returns a D-Bus path to an object related to an interface which wpa_supplicant already controls."
# Arguments s : ifname Name of the network interface, e.g., wlan0
# Returns o : interface A D-Bus path to an object representing an interface

path = proxy.GetInterface('wlan0', dbus_interface='fi.w1.wpa_supplicant1') #si dbus_interface='...' en premier --> SyntaxError: positional argument follows keyword argument
print('path de l\'objet recup avec GetInterface(): %s' % path)


#Je veux récupérer une des properties "Interfaces" définie dans l'interface fi.w1.wpa_supplicant1

props = proxy.Get('fi.w1.wpa_supplicant1', 'Interfaces', dbus_interface=dbus.PROPERTIES_IFACE) #'org.freedesktop.DBus.Properties' ou dbus.PROPERTIES_IFACE c'est pareil
print('property Interfaces %s: ' % props)










