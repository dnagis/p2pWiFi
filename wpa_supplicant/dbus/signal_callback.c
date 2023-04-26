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
 
    GVariant *devInfoDict;
    gchar *devName;
    GVariant *p_array;
    gboolean success;
    
    g_print("Dans la fonction on_signal()\n");
    g_print("   signal_name=%s\n", signal_name);
    
    if (g_strcmp0(signal_name, "DeviceFoundProperties")) {
        //le signal n est pas DeviceFoundProperties on ne va pas plus loin
        return;
    } 
    
    g_print("On continue car le signal est DeviceFoundProperties\n");
    
    //g_print ("    g_variant_get_type_string sur le variant params: %s\n", g_variant_get_type_string(params)); //(oa{sv})
    //g_print ("    g_variant_print sur le variant params: %s\n", g_variant_print (params, TRUE)); 
    
    //Dissocation de l'object path et du dict qui contient les infos qui m'interessent --> devInfoDict a{sv}
    g_print("On dissocie l'object path et l'array de dict qui sera devInfoDict\n");    
    g_variant_get (params, "(o@a{sv})", NULL, &devInfoDict);
    g_print ("g_variant_get_type_string sur le GVariant devInfoDict: %s\n", g_variant_get_type_string(devInfoDict)); //a{sv}
    g_print ("g_variant_print sur le variant devInfoDict: %s\n", g_variant_print (devInfoDict, TRUE)); 
    
    //Récupération d'entries par nom de la key se fait avec g_variant_lookup() qui nécessite un format string
    
    //DeviceName
    g_variant_lookup (devInfoDict, "DeviceName", "s", &devName);    
    g_print ("DeviceName: %s\n", devName); 
    
    //Deice Address qui apparait comme ça dans g_variant_print (devInfoDict: 'DeviceAddress': <[byte 0xe6, 0x5f, 0x01, 0x24, 0x7e, 0x23]>
    
    //cf glib_dict_playground.c pour les essais
    p_array = g_variant_lookup_value (devInfoDict, "DeviceAddress", g_variant_type_new("ay"));
    
    if (p_array == NULL) {
		g_print ("p_array est NULL\n");
		return;
	}
	
	g_print ("g_variant_print sur le GVariant p_array: %s\n", g_variant_print (p_array, TRUE)); 
    

}


int main() {
   
 	/**
 	 * Connexion à l'interface fi.w1.wpa_supplicant1.Interface.P2PDevice
 	 **/
	
	
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
   
   if (error != NULL) g_print("Erreur g_dbus_proxy_new_for_bus_sync: %s\n", error->message);
	
	/**
	 * Assignation de la callback signal received
	 */
	 
	g_signal_connect(proxy, "g-signal", G_CALLBACK(on_signal), NULL);
		
	/**
	 * 
	 * Lancement du loop
	 */
	
	GMainLoop *loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(loop);

   
   return 0;
   
}
