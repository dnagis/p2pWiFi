/**
 * Lance find sans timeout et ecoute signaux
 * 
 * 
 * gcc g_p2p_receiver_d.c -o g_p2p_receiver_d `pkg-config --cflags --libs glib-2.0 gio-2.0`
 *  
 */

#include <gio/gio.h>


GDBusProxy *proxy = NULL;


/**
 * Connect
 * 
 * Connect ( a{sv} : args ) –> s : generated_pin
 * 
 * https://w1.fi/wpa_supplicant/devel/dbus.html#dbus_p2pdevice
 * 
 * Inspiration de NetworkManager --> nm-supplicant-interface.c ligne 2824 nm_supplicant_interface_p2p_connect()
 * 
 * 
 **/ 


static void connect(gchar *peer) {

	GVariantBuilder builder;
	GError *error = NULL;
	
	g_print ("Fonction connect() pour peer = %s\n", peer);

	/**
	 * Construction du dict contenant les paramètres à passer à Connect
	 * 
	 * **/
    g_variant_builder_init(&builder, G_VARIANT_TYPE_VARDICT);

    g_variant_builder_add(&builder, "{sv}", "wps_method", g_variant_new_string("pbc"));
    g_variant_builder_add(&builder, "{sv}", "join", g_variant_new_boolean(TRUE));    
    g_variant_builder_add(&builder, "{sv}", "persistent", g_variant_new_boolean(TRUE));
    //Attention: j'ai déjà essayé de passer un GVariant* --> g_variant_builder_add() bloque sans erreur ni segfault!    
    g_variant_builder_add(&builder, "{sv}", "peer", g_variant_new_object_path(peer)); 

    //g_variant_builder_add(&builder, "{sv}", "go_intent", g_variant_new_int32(7));

	g_dbus_proxy_call_sync (proxy,
							"Connect",
							g_variant_new("(a{sv})", &builder),
							G_DBUS_CALL_FLAGS_NONE,
							-1,
							NULL,
							&error);
							
	if (error != NULL) g_print("Erreur g_dbus_proxy_call_sync pour Connect: %s\n", error->message);
	
}



static void on_signal (GDBusProxy *proxy,
                       gchar *sender_name,
                       gchar *signal_name,
                       GVariant *params,
                       gpointer user_data) {
	
	GVariant *devInfoDict;
	//gchar *devName;
	GVariant *devName;
	gchar *peer; //Attention je sais que tu auras envie de remplacer ça par GVariant * mais déjà essayé +++
	GError *error = NULL;
	GDBusProxy *peer_proxy = NULL;
	
    g_print("on_signal() signal_name=%s\n", signal_name);
    
    //g_print ("g_variant_get_type_string sur le GVariant params: %s\n", g_variant_get_type_string(params));
    //g_print ("g_variant_print sur le GVariant params: %s\n", g_variant_print (params, TRUE)); 
    
        /**if (g_strcmp0(signal_name, "GONegotiationRequest") == 0) {
        g_print ("g_variant_get_type_string sur le GVariant params: %s\n", g_variant_get_type_string(params)); //oqy
        g_print ("g_variant_print sur le GVariant params: %s\n", g_variant_print (params, TRUE));
        //(objectpath '/fi/w1/wpa_supplicant1/Interfaces/0/Peers/9cb6d0172c8f', uint16 4, byte 0x07)
        

        
        
    }*/ 
    
    
    
	
    if (g_strcmp0(signal_name, "DeviceFoundProperties") == 0) {
        g_variant_get (params, "(oa{sv})", &peer, NULL);  
        g_print("DeviceFoundProperties o=%s\n", peer); // /fi/w1/wpa_supplicant1/Interfaces/0/Peers/e2bb9ed5bb53
        
        peer_proxy = g_dbus_proxy_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
											G_DBUS_PROXY_FLAGS_NONE,
											NULL, /* GDBusInterfaceInfo */
											"fi.w1.wpa_supplicant1", //name,
											peer, //object_path,
											"fi.w1.wpa_supplicant1.Peer", //interface_name
											NULL, /* GCancellable */
											&error);
											
	if (error != NULL) g_print("Erreur g_dbus_proxy_new_for_bus_sync: %s\n", error->message);
	
	//Pas d'erreur, il semblerait que j'ai accès au peer		
	
	//https://stackoverflow.com/questions/47095118/c-gdbus-how-to-fetch-the-property-of-interface-using-gdbus-library									
	
	
	//devName = g_dbus_proxy_get_cached_property(peer_proxy, "ZOB");
	devName = g_dbus_proxy_get_cached_property(peer_proxy, "DeviceName");
	
	
	g_print("devName: %s\n", &devName);
        

    }

}


int main() {
	
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
	 * https://w1.fi/wpa_supplicant/devel/dbus.html#dbus_p2pdevice
	 * 
	 * Construction des arguments pour la méthode Find: inspiration: network manager: nm-supplicant-interface.c
	 * */

	//guint timeout = 120; //s. Au terme --> P2P-FIND-STOPPED. Pas mandatory.
	GVariantBuilder methodParms;
		
	g_variant_builder_init(&methodParms, G_VARIANT_TYPE_VARDICT);
	//g_variant_builder_add(&methodParms, "{sv}", "Timeout", g_variant_new_int32(timeout)); 
		
	g_dbus_proxy_call_sync (proxy,
							"Find",
							g_variant_new("(a{sv})", &methodParms),
							G_DBUS_CALL_FLAGS_NONE,
							-1,
							NULL,
							&error);
							
	if (error != NULL) g_print("Erreur g_dbus_proxy_call_sync pour Find: %s\n", error->message);
		
	/**
	 * Lancement du loop
	 * */
	
	GMainLoop *loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(loop);
   
	return 0;
   
}
