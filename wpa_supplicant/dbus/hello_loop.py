#!/usr/bin/env python3

#Juste avoir un loop, parce que le loop des scripts dans wpa_supplicant-2.9/wpa_supplicant/examples/p2p/ bouffe du CPU
#et n'a pas l'air d'être dans les bonnes pratiques du moment
#https://dbus.freedesktop.org/doc/dbus-python/tutorial.html#setting-up-an-event-loop
#https://stackoverflow.com/questions/56684286/how-to-gracefully-terminate-a-glib-main-loop-from-python


from dbus.mainloop.glib import DBusGMainLoop
from gi.repository import GLib
import signal

def sigint_handler(sig, frame):
    if sig == signal.SIGINT:
        loop.quit()
    else:
        raise ValueError("Undefined handler for '{}'".format(sig))



if __name__ == '__main__':
	
	DBusGMainLoop(set_as_default=True)

	print("Running ton loop...")
	
	signal.signal(signal.SIGINT, sigint_handler)
	loop = GLib.MainLoop()
	loop.run()

