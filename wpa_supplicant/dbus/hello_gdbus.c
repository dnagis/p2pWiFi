/**
 * 
Hello world accès dbus via gdbus

Doc gtk gio "Migrating to gdbus"
https://docs.gtk.org/gio/migrating-gdbus.html

gcc hello_gdbus.c -o hello_gdbus `pkg-config --cflags --libs glib-2.0 gio-2.0`

g_dbus_proxy_new_for_bus_sync() a l'air d'être dans la librairie libgio-2.0.so.0

* 
**/

#include <gio/gio.h>


int main() {
   
   
   GDBusProxy *proxy = NULL;
   GError *error = NULL;
   proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
                                       G_DBUS_PROXY_FLAGS_NONE,
                                       NULL, /* GDBusInterfaceInfo */
                                       "org.freedesktop.Accounts",
                                       "/org/freedesktop/Accounts",
                                       "org.freedesktop.Accounts",
                                       NULL, /* GCancellable */
                                       &error);
   
   
   return 0;
   
}
