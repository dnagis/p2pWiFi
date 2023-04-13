/**
 * 
Compil native 86_64: gcc hello_loop.c -o hello_loop `pkg-config --cflags --libs glib-2.0`

export PATH=$PATH:/initrd/mnt/dev_save/rpi/cross/bin/
export PKG_CONFIG_LIBDIR=/initrd/mnt/dev_save/rpi/cross/aarch64-linux-gnu/lib/pkgconfig
export PKG_CONFIG_SYSROOT_DIR=/initrd/mnt/dev_save/rpi/cross/aarch64-linux-gnu

aarch64-linux-gnu-gcc hello_loop.c -o hello_loop `pkg-config --cflags --libs glib-2.0`


https://docs.gtk.org/glib/struct.MainLoop.html
* 
**/

#include <glib.h>

int main() {
   
   g_print("Avant le lancement du loop\n");
   GMainLoop *loop = g_main_loop_new(NULL, FALSE);
   g_main_loop_run(loop);
   g_print("Apres le lancement du loop\n");
   return 0;
   
}
