//gcc `pkg-config --cflags --libs glib-2.0 gio-2.0` -o g_variant-access-data g_variant-access-data.c




#include <glib.h>
#include <gio/gio.h>

int
main (void)
{

	GVariant *data;
	gint value = 1;
	gint max = 3;
	
	/* type (oa{sv}) */
	data = g_variant_new_parsed ("(%o, {'value': <%i>, 'max': <%i>})", "/object/path", value, max);
	                             
	g_print ("g_variant_get_type_string sur le variant data: %s\n", g_variant_get_type_string(data));
	g_print ("g_variant_print sur le variant data: %s\n", g_variant_print (data, TRUE)); 

	GVariant *params;
	GVariant *p_brightness;
	gchar *obj;
	gint p_max;
	
	g_variant_get (data, "(o@a{?*})", &obj, &params);
	
	g_print ("object_path: %s\n", obj);
	g_print ("g_variant_print sur le variant params: %s\n", g_variant_print (params, TRUE));
	
	g_variant_lookup (params, "value", "i", &p_max);
	g_print ("value: %d\n", p_max);
	
	/*p_brightness = g_variant_lookup_value (params, "brightness", G_VARIANT_TYPE_VARDICT);
	g_variant_lookup (p_brightness, "max", "i", &p_max);
	g_print ("max: %d\n", p_max);*/


  return 0;

}
