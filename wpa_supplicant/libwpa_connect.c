/*
 * Octobre 2021
 * 
 * client: se connecte à wpa_supplicant via librairie /usr/lib64/libwpa_client.so
 * 
 * Passe en mode listen pour être discoverable, puis écoute les events, puis si event matche str: connect
 * testé initialement avec en face le code android samples WifiDirectDemo
 *  
 * 
 * Basé sur wpa_supplicant/libwpa_test.c 
 * 
 * 
 * gcc libwpa_connect.c -I...path_to.../wpa_supplicant-2.9/src -I...path_to.../wpa_supplicant-2.9/src/utils -lwpa_client
 * 
 * X-compil:
 * export PATH=$PATH:/initrd/mnt/dev_save/rpi/cross/bin/
 * arm-linux-gnueabihf-gcc libwpa_connect.c -I...path_to.../wpa_supplicant-2.9/src -I...path_to.../wpa_supplicant-2.9/src/utils -lwpa_client
 */
 
 
#include <string.h>

#include "includes.h"
#include "common/wpa_ctrl.h"

int main(int argc, char *argv[])
{
	struct wpa_ctrl *ctrl;
	

	char buf[4096];
	size_t len;	


	ctrl = wpa_ctrl_open("/var/run/wpa_supplicant/p2p-dev-wlan0");
	if (!ctrl)
		return -1;
	if (wpa_ctrl_attach(ctrl) == 0){
		
		
		len = sizeof(buf) - 1; //il faut réattribuer avant chaque utilisation, sinon segfault ou reste à la première valeur
		wpa_ctrl_request(ctrl, "P2P_LISTEN", 10, buf, &len, NULL);
		printf("reponse: len=%d , %s \n",len,buf);
		
		//ecouter
		while (1) {
		
		printf("Boucle debut\n");
			
			if (wpa_ctrl_pending(ctrl)) {


			
			printf("on a un msg\n");

			len = sizeof(buf) - 1;
			wpa_ctrl_recv(ctrl, buf, &len);
			
			printf("event: len=%d , %s \n",len,buf);
			
			if (strstr(buf, "P2P-GO-NEG-REQUEST d6:c9:4b:81:c2:73")) {

				
				printf("On a une demande de connexion\n");
				
				
				//wpa_ctrl_request(ctrl, "P2P_CONNECT d6:c9:4b:81:c2:73 PBC GO_INTENT=0", 45, buf, &len, NULL); --> FAIL-INVALID-PIN
				len = sizeof(buf) - 1;
				wpa_ctrl_request(ctrl, "P2P_CONNECT d6:c9:4b:81:c2:73 pbc go_intent=0", 45, buf, &len, NULL);
				printf("reponse: len=%d , %s \n",len,buf);
				}
			}
		
		
		sleep(1);	
		}
		
		
	}
	

	
	
	wpa_ctrl_detach(ctrl);
	wpa_ctrl_close(ctrl);

	return 0;
}
