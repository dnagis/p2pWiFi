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
 * INCLUDES="-I/initrd/mnt/dev_save/packages/wpa_supplicant-2.9/src -I/initrd/mnt/dev_save/packages/wpa_supplicant-2.9/src/utils"
 * gcc libwpa_connect.c $INCLUDES -lwpa_client
 * 
 * X-compil:
 * export PATH=$PATH:/initrd/mnt/dev_save/rpi/cross/bin/
 * INCLUDES="-I/initrd/mnt/dev_save/packages/wpa_supplicant-2.9/src -I/initrd/mnt/dev_save/packages/wpa_supplicant-2.9/src/utils"
 * arm-linux-gnueabihf-gcc libwpa_connect.c $INCLUDES -lwpa_client -o libwpa_connect
 * 
 */
 
 
#include <string.h>
#include <stdbool.h>

#include "includes.h"
#include "common/wpa_ctrl.h"


struct wpa_ctrl *ctrl;
char buf[4096];
size_t len;	



/**
 * Compare addr avec une liste d'adresses connues, return true si au moins une occurence
 * 
 */
bool chkmacaddr(char *addr)
{
	char *known_addrs[] = {
	"1a:f0:e4:11:ef:ba", //redmi note 4
	"d6:c9:4b:81:c2:73", //moto Z3 play
	"7e:42:6e:7f:a3:da", //Poco M3
	"e2:c3:77:fd:59:5c", //Galaxy Tab S5e
	"ENDOFARRAY"};
	int i=0;
		
	while (strcmp(known_addrs[i]	, "ENDOFARRAY")) {
		if (strstr(addr, known_addrs[i])) return true;
		i++;
	};
	
	return false;
}


void lance_listen(){
	printf("On lance cmd=P2P_LISTEN...\n");
	len = sizeof(buf) - 1; //il faut réattribuer avant chaque utilisation, sinon segfault ou reste à la première valeur
	wpa_ctrl_request(ctrl, "P2P_LISTEN", 10, buf, &len, NULL);
	printf("reponse: len=%d , %.*s\n",len,len,buf); //%.*s => n'imprimer que len caractères (*='precision specifier' pour printf)
}




int main(int argc, char *argv[])
{
	
	ctrl = wpa_ctrl_open("/var/run/wpa_supplicant/p2p-dev-wlan0");
	if (!ctrl)
		return -1;
	if (wpa_ctrl_attach(ctrl) == 0){
		
		lance_listen();
		
		while (1) {
		
		//printf("Boucle debut\n");
			
			if (wpa_ctrl_pending(ctrl)) {

			len = sizeof(buf) - 1;
			wpa_ctrl_recv(ctrl, buf, &len);
			
			printf("Rx event: len=%d , %.*s\n",len,len,buf); //%.*s => n'imprimer que len caractères (*='precision specifier' pour printf)
			
			//On veut répondre à une demande de connexion du type:
			//<3>P2P-GO-NEG-REQUEST 1a:f0:e4:11:ef:ba dev_passwd_id=4 go_intent=64:11:ef:ba bssid=1a:f0:e4:11:ef:ba unknown-networkonfig_methods=0x188 dev_capab=0x25 group_capab=0x0 new=1 
			if (strstr(buf, "P2P-GO-NEG-REQUEST")) {
				printf("Detection Rx P2P-GO-NEG-REQUEST\n");
				
				//Récupération de l'addr mac du requester
				char * raddr; //requester mac address
				char * token; //utilisation de strtok est un peu spéciale
				token = strtok(buf, " "); //à ce stade token = <3>P2P-GO-NEG-REQUEST
				raddr = strtok(NULL, " "); //oui c'est hyper chelou la manière d'avancer dans buf... là on est au 2ème token
				printf("requester mac addr=%s\n", raddr);
			
				//on checke si addr du requester connue, fonction perso
				if (chkmacaddr(raddr)){
					 printf("chkmacaddr a return true donc addresse: %s connue\n", raddr);
				
					//On répond à la demande de connexion:
					//Syntaxe pour une commande à passer(la casse est... particulière):
					//wpa_ctrl_request(ctrl, "P2P_CONNECT d6:c9:4b:81:c2:73 PBC GO_INTENT=0", 45, buf, &len, NULL); --> donne FAIL-INVALID-PIN il faut minuscules mais ONLY après la macaddr!
					char cmd[45] = "P2P_CONNECT ";
					strcat(cmd, raddr);
					strcat(cmd, " pbc go_intent=0");
					printf("on va envoyer la commande: %s\n", cmd);
					len = sizeof(buf) - 1;
					wpa_ctrl_request(ctrl, cmd, 45, buf, &len, NULL);
					printf("reponse: len=%d , %.*s\n",len,len,buf); //%.*s => n'imprimer que len caractères (*='precision specifier' pour printf)
					
					}					
				}
			
			//Rx: P2P-GROUP-REMOVED -->	on relance p2p_listen
			if (strstr(buf, "P2P-GROUP-REMOVED")) {
				printf("Detection Rx P2P-GROUP-REMOVED\n");
				lance_listen();
				}
				
				
			}
		
		
		sleep(1);	
		}
		
		
	}
	

	
	
	wpa_ctrl_detach(ctrl);
	wpa_ctrl_close(ctrl);

	return 0;
}
