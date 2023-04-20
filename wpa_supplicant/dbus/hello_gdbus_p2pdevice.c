/**
 * Accéder à l'interface P2PDevice en gdbus avec possibilité de vérification objective
 * 
 * Fonctionnement:
 * Appel de la méthode Find --> contrôle dans une autre fenêtre avec wpa_cli (p2p-dev-wlan0 est par défaut)
 * 
 * gcc gdbus_p2pdevice.c -o gdbus_p2pdevice `pkg-config --cflags --libs glib-2.0 gio-2.0`
 * 
 * DBUS_SYSTEM_BUS_ADDRESS=unix:path=/usr/var/run/dbus/system_bus_socket ./gdbus_p2pdevice
 * 
 */

#include <gio/gio.h>


int main() {
   
 	/**
 	 * Connexion à l'interface fi.w1.wpa_supplicant1.Interface.P2PDevice
 	 * 
 	 * Pas intuitif le contenu des 3 arguments: name, object_path, et interface_name
 	 * nb: object path est hardcodé, mais c'est le résultat de l'appel de GetInterface à la racine des interfaces fi.w1.wpa_supplicant1
 	 * (cf hello_gdbus_wpas.c)
 	 * 
 	 * Replissage des 3 args trouvé grâce à une recherche Google 'GDBusProxy P2PDevice':
 	 * 
 	 * https://stackoverflow.com/questions/36596404/registering-a-signal-handler-with-wpa-supplicant-p2p-dbus-interface  
 	 */
	
	
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
   
   
   if (error != NULL)
		{
		g_print("Erreur g_dbus_proxy_new_for_bus_sync: %s\n", error->message);
		}
	
	/**
	 * 
	 * Appel de Find() sur fi.w1.wpa_supplicant1.Interface.P2PDevice
	 * 
	 * Pour la syntaxe: NetworkManger de chez gnome / freedesktop
	 * https://download.gnome.org/sources/NetworkManager/1.43/NetworkManager-1.43.5.tar.xz
	 * 
	 * NetworkManager/src/cors/supplicant/nm-supplicant-interface.c
	 */
	 
	 
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
	
	if (error != NULL)
		{
    g_print("Erreur g_dbus_proxy_call_sync: %s\n", error->message);
		}
		
	/**
	 * 
	 * Lancer le loop
	 * 
	 * 
	 */
	
	
	GMainLoop *loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(loop);

   
   return 0;
   
}
