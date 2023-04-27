/**
 * Connexion p2p comme avec p2p_connect.py
 * 
 * gcc g_p2p_connect.c -o g_p2p_connect `pkg-config --cflags --libs glib-2.0 gio-2.0`
 * 
 * 
 */

#include <gio/gio.h>

gchar *dev_searched = "RPi4";
GDBusProxy *proxy = NULL;


/**
 * Connect
 * 
 * Connect ( a{sv} : args ) –> s : generated_pin
 * 
 * https://w1.fi/wpa_supplicant/devel/dbus.html#dbus_p2pdevice
 * 
 * NetworkManager --> nm-supplicant-interface.c ligne 2824 nm_supplicant_interface_p2p_connect()
 * 
 * 
 * 
 * */

static void connect(GVariant *peer) {
	
	
	GVariantBuilder builder;
	GError *error = NULL;
	
	g_print ("Fonction connect() pour peer = %s\n", peer);
	
	
	
	g_print ("Dans connect() g_variant_get_type_string sur le GVariant peer: %s\n", g_variant_get_type_string(peer));
	
    g_variant_builder_init(&builder, G_VARIANT_TYPE_VARDICT);

    g_variant_builder_add(&builder, "{sv}", "wps_method", g_variant_new_string("pbc"));
    g_variant_builder_add(&builder, "{sv}", "join", g_variant_new_boolean(TRUE));    
    
    
    /**Bloque au runtime ici
     * 
     * */
    
    //g_variant_builder_add(&builder, "{sv}", "peer", g_variant_new_object_path(peer)); //il faut gchar* pour avoir qq chose
    g_variant_builder_add(&builder, "{sv}", "peer", peer);
	




    //g_variant_builder_add(&builder, "{sv}", "persistent", g_variant_new_boolean(TRUE));
    //g_variant_builder_add(&builder, "{sv}", "go_intent", g_variant_new_int32(7));

	g_print ("on est avant le lancement de g_dbus_proxy_call_sync() pour Connect\n");

	g_dbus_proxy_call_sync (proxy,
							"Connect",
							g_variant_new("(a{sv})", &builder),
							G_DBUS_CALL_FLAGS_NONE,
							-1,
							NULL,
							&error);
							
    g_print ("on est apres le lancement de g_dbus_proxy_call_sync() pour Connect\n");
							
	if (error != NULL) g_print("Erreur g_dbus_proxy_call_sync pour Connect: %s\n", error->message);
	
}

static void on_signal (GDBusProxy *proxy,
                       gchar *sender_name,
                       gchar *signal_name,
                       GVariant *params,
                       gpointer user_data) {
	
	GVariant *devInfoDict;
	gchar *devName;
	GVariant *peer;  
	
    g_print("on_signal() signal_name=%s\n", signal_name);
    
    //g_print ("g_variant_get_type_string sur le GVariant params: %s\n", g_variant_get_type_string(params));
    //g_print ("g_variant_print sur le GVariant params: %s\n", g_variant_print (params, TRUE)); 
    
    //Le device name ('RPi4') n'est présent que quand le signal = DeviceFoundProperties
    if (g_strcmp0(signal_name, "DeviceFoundProperties") == 0) {
        g_print("signal_name = DeviceFoundProperties\n");
        
        g_print("on cherche %s\n", dev_searched);
        
        //DeviceName
        g_variant_get (params, "(o@a{sv})", &peer, &devInfoDict);
        g_variant_lookup (devInfoDict, "DeviceName", "s", &devName);    
		g_print ("DeviceName: %s\n", devName); 
		
		//Object Path https://docs.gtk.org/glib/gvariant-format-strings.html à l'exemple strings
		if (peer == NULL) {
			g_print ("peer est NULL\n");
				} else {
			g_print ("peer = %s\n", peer);
			}
		
		
		g_print ("Dans on_signal() g_variant_get_type_string sur le GVariant peer: %s\n", g_variant_get_type_string(peer)); //SegFault


				
		//if (g_strcmp0(devName, dev_searched) == 0) {
			g_print ("On a trouve: %s on lance un connect...\n", dev_searched); 
			connect(peer);
		//	}
		
        
        
       
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
	 * Construction des arguments pour la méthode Find: network manager: nm-supplicant-interface.c
	 * */

	guint timeout = 60; //Timeout (s) au terme --> P2P-FIND-STOPPED
	GVariantBuilder methodParms;
		
	g_variant_builder_init(&methodParms, G_VARIANT_TYPE_VARDICT);
	g_variant_builder_add(&methodParms, "{sv}", "Timeout", g_variant_new_int32(timeout)); 
		
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
