/**
 * Connexion p2p comme avec p2p_connect.py
 * 
 * gcc g_p2p_connect.c -o g_p2p_connect `pkg-config --cflags --libs glib-2.0 gio-2.0`
 * 
 * 
 */

#include <gio/gio.h>

static void on_signal (GDBusProxy *proxy,
                       gchar *sender_name,
                       gchar *signal_name,
                       GVariant *params,
                       gpointer user_data) {
 
    g_print("on_signal() signal_name=%s\n", signal_name);

    }


int main() {
   
	
	GDBusProxy *proxy = NULL;
	GDBusProxy *p2p_proxy = NULL;
	GError *error = NULL;

	proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
											G_DBUS_PROXY_FLAGS_NONE,
											NULL, /* GDBusInterfaceInfo */
											"fi.w1.wpa_supplicant1", //name,
											"/fi/w1/wpa_supplicant1/Interfaces/0", //object_path,
											"fi.w1.wpa_supplicant1.Interface.P2PDevice", //interface_name
											NULL, /* GCancellable */
											&error);
   
	if (error != NULL) g_print("Erreur g_dbus_proxy_new_for_bus_sync: %s\n", error->message);
   
   	/**
	 * Assignation de la callback signal received
	 */
	 
	g_signal_connect(proxy, "g-signal", G_CALLBACK(on_signal), NULL);	
	
	/**
	 * Appel de Find() sur fi.w1.wpa_supplicant1.Interface.P2PDevice
	 * */
	 
	//Construction des arguments pour Find: Après Timeout s: P2P-FIND-STOPPED
	guint timeout = 30;
	GVariantBuilder builder;
	g_variant_builder_init(&builder, G_VARIANT_TYPE_VARDICT);
    g_variant_builder_add(&builder, "{sv}", "Timeout", g_variant_new_int32(timeout)); 	
		
	g_dbus_proxy_call_sync (proxy,
							"Find",
							g_variant_new("(a{sv})", &builder),
							G_DBUS_CALL_FLAGS_NONE,
							-1,
							NULL,
							&error);
	
	if (error != NULL) g_print("Erreur g_dbus_proxy_call_sync: %s\n", error->message);
		
	/**
	 * Lancement du loop
	 * */
	
	GMainLoop *loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(loop);
   
	return 0;
   
}
