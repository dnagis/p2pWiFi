/**
 * Travail sur object path et variant
 * 
 * gcc object_path_type.c -o object_path_type `pkg-config --cflags --libs glib-2.0 gio-2.0`
 * 
 * */



#include <gio/gio.h>

int
main (void)
{
	GVariant *value;
	gchar *result;

	value = g_variant_new ("o", "/must/be/a/valid/path");
	
	
	g_print ("g_variant_get_type_string sur le GVariant value: %s\n", g_variant_get_type_string(value)); // --> o
	

	g_variant_get (value, "o", &result);
	//g_print ("Affichage '%s'\n", result);

	GVariantBuilder builder;
	
	
    g_variant_builder_init(&builder, G_VARIANT_TYPE_VARDICT);

    g_variant_builder_add(&builder, "{sv}", "wps_method", g_variant_new_string("pbc")); 
    g_variant_builder_add(&builder, "{sv}", "join", g_variant_new_boolean(TRUE));    
	g_variant_builder_add(&builder, "{sv}", "peer", value);
	
	g_print ("Fin du programme\n");
	
	return 0;

}
