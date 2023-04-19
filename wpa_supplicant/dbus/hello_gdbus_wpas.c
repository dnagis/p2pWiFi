/**
 * Appel de la méthode GetInterface (interface au sens network) sur l'interface (au sens dbus) fi.w1.wpa_supplicant1

gcc hello_gdbus_wpas.c -o hello_gdbus_wpas `pkg-config --cflags --libs glib-2.0 gio-2.0`

aarch64-linux-gnu-gcc hello_gdbus_wpas.c -o hello_gdbus_wpas `pkg-config --cflags --libs glib-2.0 gio-2.0`

DBUS_SYSTEM_BUS_ADDRESS=unix:path=/usr/var/run/dbus/system_bus_socket ./hello_gdbus_wpas

* 
**/

#include <gio/gio.h>


int main() {
   
   
   GDBusProxy *proxy = NULL;
   GError *error = NULL;
   proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
                                       G_DBUS_PROXY_FLAGS_NONE,
                                       NULL, /* GDBusInterfaceInfo */
                                       "fi.w1.wpa_supplicant1",
                                       "/fi/w1/wpa_supplicant1",
                                       "fi.w1.wpa_supplicant1",
                                       NULL, /* GCancellable */
                                       &error);
   
   
   if (error != NULL)
  {
    g_print("Erreur g_dbus_proxy_new_for_bus_sync: %s\n", error->message);
  }
  
  //Appel de la method GetInterface sur le proxy
  //https://docs.gtk.org/gio/method.DBusProxy.call_sync.html
  //https://docs.gtk.org/glib/gvariant-format-strings.html
	
  GVariant *wlan0_path = NULL;
  
  wlan0_path = g_dbus_proxy_call_sync (
							proxy,
							"GetInterface",
							g_variant_new("(s)","wlan0"),
							G_DBUS_CALL_FLAGS_NONE,
							-1,
							NULL,
							&error);
	
	if (error != NULL)
  {
    g_print("Erreur g_dbus_proxy_call_sync: %s\n", error->message);
  }
  
 	g_print ("Variant type: '%s'\n", g_variant_print (wlan0_path, TRUE)); 
	
	//g_assert_true (g_variant_type_equal (g_variant_get_type (wlan0_path), G_VARIANT_TYPE_STRING));
	//g_assert_true (g_variant_type_equal (g_variant_get_type (wlan0_path), G_VARIANT_TYPE_OBJECT_PATH_ARRAY));
	


	
	
  

   
   return 0;
   
}
