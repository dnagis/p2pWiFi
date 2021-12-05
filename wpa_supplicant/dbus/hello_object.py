#!/usr/bin/env python3


#simple_object
#dbus et les objets, les opath (object path), les interfaces

#Le tutorial spécial dbus-python MAIS utilise org.freedesktop.NetworkManager que je n'ai pas
#https://dbus.freedesktop.org/doc/dbus-python/tutorial.html

#Le tutorial pydbus (pydbus utilise à mon avis une librairie que je n'ai pas), 
#	fait référence à org.freedesktop.DBus que tout le monde a...
#https://wiki.python.org/moin/DbusExamples 



import dbus


bus = dbus.SystemBus()


proxy = bus.get_object(
			'org.freedesktop.DBus', # Bus name
			'/org/freedesktop/DBus' # Object path
			)
# proxy is a dbus.proxies.ProxyObject


#L'API DBus https://dbus.freedesktop.org/doc/dbus-java/api/org/freedesktop/DBus.html 
#me dit qu'il y a une méthode ListNames() --->         Lists all connected names on the Bus.

print(proxy.ListNames(dbus_interface='org.freedesktop.DBus'))







