#!/usr/bin/env python3


from gi.repository import GLib

import dbus
import dbus.service
import dbus.mainloop.glib


if __name__ == '__main__':
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

    system_bus = dbus.SystemBus()
    #name = dbus.service.BusName("com.example.SampleService", session_bus)
    #object = SomeObject(session_bus, '/SomeObject')

    mainloop = GLib.MainLoop()
    print("Running example service.")
    #print(usage)
    mainloop.run()
