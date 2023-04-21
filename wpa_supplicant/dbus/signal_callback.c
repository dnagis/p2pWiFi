/**
 * Recevoir des signaux sur l'interface P2PDevice
 * 
 * Je souhaite recevoir des signaux comme P2P-DEVICE-FOUND
 * 
 * Pour générer des signaux, aller en wpa_cli et faire p2p_find
 * 
 * 
 * gcc signal_callback.c -o signal_callback `pkg-config --cflags --libs glib-2.0 gio-2.0`
 * 
 */

#include <gio/gio.h>


//https://stackoverflow.com/questions/36596404/registering-a-signal-handler-with-wpa-supplicant-p2p-dbus-interface
static void on_signal (GDBusProxy *proxy,
                       gchar *sender_name,
                       gchar *signal_name,
                       GVariant *params,
                       gpointer user_data) {
 
    g_print("Dans la fonction on_signal()\n");
    g_print("   signal_name=%s\n", signal_name);
    
    if (g_strcmp0(signal_name, "DeviceFoundProperties")) {
        //le signal n est pas DeviceFoundProperties on ne va pas plus loin
        return;
    } 
    
    g_print("On continue car le signal est DeviceFoundProperties\n");
    
    g_print ("    g_variant_get_type_string sur le variant params: %s\n", g_variant_get_type_string(params)); //(oa{sv})
    
    g_print ("    g_variant_print sur le variant params: %s\n", g_variant_print (params, TRUE)); 
    
    if (g_variant_is_of_type(params, G_VARIANT_TYPE("(o)"))) {
                g_print("   variant_type=(o)\n");
            }
            
    

}


int main() {
   
 	/**
 	 * Connexion à l'interface fi.w1.wpa_supplicant1.Interface.P2PDevice
 	 * 
 	 */
	
	
	GDBusProxy *proxy = NULL;
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
	 * 
	 */
	 
	 
	     g_signal_connect(proxy,
                     "g-signal",
                     G_CALLBACK(on_signal), // stub func that does something simple
                     NULL);
	 

		
	/**
	 * 
	 * Lancer le loop
	 */
	
	
	GMainLoop *loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(loop);

   
   return 0;
   
}
