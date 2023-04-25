/**
 * Accéder à un byte array contenu dans un dict a{sv}
 * 
 * gcc `pkg-config --cflags --libs glib-2.0 gio-2.0` -o glib_dict_playground glib_dict_playground.c
 * 
 * https://stackoverflow.com/questions/70581948/extract-array-of-bytes-from-gvariant
 * https://stackoverflow.com/questions/41516615/how-do-you-create-a-dictionary-that-maps-onto-an-array-in-glib
 * https://docs.gtk.org/glib/gvariant-format-strings.html#dictionaries
 * 
 * */



#include <gio/gio.h>

int
main (void)
{

	GVariantBuilder *b;
	GVariant *dict;
	GVariant *byte_array_v;	
	GVariant *p_array;
	
	guint8 byte_array[] = { 1, 2, 3, 4, 5 };

	byte_array_v = g_variant_new_from_data(G_VARIANT_TYPE ("ay"),
                                  byte_array,
                                  G_N_ELEMENTS(byte_array),
                                  TRUE,
                                  NULL,
                                  NULL);
	
	
	//Construction d'un GVariant donc le type est a{sv}
	
	b = g_variant_builder_new (G_VARIANT_TYPE ("a{sv}"));
	g_variant_builder_add (b, "{sv}", "name", g_variant_new_string ("foo"));
	g_variant_builder_add (b, "{sv}", "timeout", g_variant_new_int32 (10));	
	g_variant_builder_add (b, "{sv}", "address", byte_array_v);	
	dict = g_variant_builder_end (b);
	
	g_print ("g_variant_get_type_string sur le GVariant dict: %s\n", g_variant_get_type_string(dict)); //a{sv}	
	
	g_print ("g_variant_print sur le GVariant dict: %s\n", g_variant_print (dict, TRUE)); // {'name': <'foo'>, 'timeout': <10>, 'address': <[byte 0x01, 0x02, 0x03, 0x04, 0x05]>}
	
	
	//Essayer d'obtenir un pointeur vers l'array avec g_variant_lookup_value()
	//https://docs.gtk.org/glib/method.Variant.lookup_value.html
	p_array = g_variant_lookup_value (dict, "address", g_variant_type_new("ay"));
	
	if (p_array == NULL) {
		g_print ("p_array est NULL\n");
		return 1;
	}
	
	g_print ("g_variant_print sur le GVariant p_array: %s\n", g_variant_print (p_array, TRUE)); //[byte 0x01, 0x02, 0x03, 0x04, 0x05]
	
	
	
	return 0;

}
