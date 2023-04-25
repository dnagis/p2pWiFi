/**
 * Accéder au données récupérées dans un variant
 * type (oa{sv})
 * 
 * https://docs.gtk.org/glib/gvariant-format-strings.html#dictionaries
 * 
 * 
 * gcc `pkg-config --cflags --libs glib-2.0 gio-2.0` -o g_variant-access-data g_variant-access-data.c
 * 
 * */



#include <gio/gio.h>

int
main (void)
{

	GVariant *data, *params, *p_brightness;
	gint value = 1;
	gint max = 3;
	gchar *model_in = "RPi4"; //Pour un exemple de string
	gint p_max;	
	gchar *model_out;
	
	//g_print ("la string = %s\n", model_in);

	
	/* Création d'un variant type (oa{sv}) */
	data = g_variant_new_parsed ("(%o, {'value': <%i>, 'max': <%i>, 'model': <%s>})", "/object/path", value, max, model_in);
	                             
	g_print ("g_variant_get_type_string sur le variant data: %s\n", g_variant_get_type_string(data));
	g_print ("g_variant_print sur le variant data: %s\n", g_variant_print (data, TRUE)); 
	
	
	/**
	 * coeur du fonctionnement: dissocie l'objet et l'array de dict (ce qui m interesse le plus)
	 * 
	 * g_variant_get (data, "(o@a{sv})", &obj, &params); //si tu veux récupérer un gchar *obj qui te donnera l object path (printable avec g_print ("object_path: %s\n", obj);)
	 * 
	 * g_variant_get (data, "(o@a{?*})", &obj, &params); //utilisé dans l exemple source https://docs.gtk.org/glib/gvariant-format-strings.html#dictionaries dont le but est d extraire un nested dict (oa{sa{sv})
	 * 
	 * 
	**/
	g_variant_get (data, "(o@a{sv})", NULL, &params); 
	
	
	
	g_print ("g_variant_get_type_string sur le variant params: %s\n", g_variant_get_type_string(params)); //a{sv}
	g_print ("g_variant_print sur le variant params: %s\n", g_variant_print (params, TRUE));
	
	g_variant_lookup (params, "max", "i", &p_max);
	//g_print ("g_variant_get_type_string sur p_max: %s\n", g_variant_get_type_string(p_max)); //impossible à la compil: g_variant_get_type_string attends un GVariant*
	g_print ("max: %d\n", p_max);
	
	g_variant_lookup (params, "model", "s", &model_out);
	g_print ("model: %s\n", model_out);
	
	
	
	
	/** Pour accéder à des nested dicts type (oa{sa{sv})
	 * 
	 * 
	p_brightness = g_variant_lookup_value (params, "brightness", G_VARIANT_TYPE_VARDICT);
	g_variant_lookup (p_brightness, "max", "i", &p_max);
	g_print ("max: %d\n", p_max);*/


  return 0;

}
