#!/usr/bin/env python3


#Hello_p2p
#Je pars de hello_wpas et j'essaie d'avoir le code le plus simple possible pour accéder au P2P et lancer p2p_find

#https://dbus.freedesktop.org/doc/dbus-python/tutorial.html
#p2p_find.py



import dbus




bus = dbus.SystemBus()

#Récupération de wpas: un proxy vers /fi/w1/wpa_supplicant1, de type: dbus.proxies.ProxyObject
wpas = bus.get_object('fi.w1.wpa_supplicant1','/fi/w1/wpa_supplicant1') 

#Appel de la methode GetInterface() définie dans l'API dbus de wpa_supplicant (https://w1.fi/wpa_supplicant/devel/dbus.html#dbus_main)
wlan0_path = wpas.GetInterface('wlan0', dbus_interface='fi.w1.wpa_supplicant1')


print(wlan0_path) #/fi/w1/wpa_supplicant1/Interfaces/0


#Je récupère un proxy vers wlan0, en utilisant l'objet proxy wpas
#Je suppose que wlan0_path est toujours = à /fi/w1/wpa_supplicant1/Interfaces/0 , ça évite deux lignes de code (bus.get_object() et GetInterface())
wlan0_object = bus.get_object('fi.w1.wpa_supplicant1', wlan0_path)

#Je récupère une objet de type dbus.Interface qui me permettra d'appeler les methodes dessus
p2p_interface = dbus.Interface(wlan0_object, dbus_interface='fi.w1.wpa_supplicant1.Interface.P2PDevice')



#Appel de Find(), possible de vérifier le fonctionnement avec une autre fenêtre wpa_cli -i p2p-dev-wlan0 
P2PFindDict = dbus.Dictionary({'Timeout':int(30)})
p2p_interface.Find(P2PFindDict)







