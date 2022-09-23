/*
 * 
 * 
adb uninstall vvnx.P2P_wifi && \
adb install out/target/product/generic_arm64/system/app/P2P_wifi/P2P_wifi.apk && \
adb shell pm grant vvnx.P2P_wifi android.permission.ACCESS_FINE_LOCATION
* 
* 
 * Pour P2P côté linux voir LOG_p2p_wpa_supplicant 
 * 
 * ###Première phase: primo-connexion
 * 
 * le point de départ de la connexion = discoverPeers(). J'ai plusieurs fois essayé des connect() sans passer par discoverPeers avant (même si device connu, cad reconnexion)
 * , ça na jamais marché (et cohérent avec ce que j'ai lu). Ensuite le broadcastReceiver reçoit WIFI_P2P_PEERS_CHANGED_ACTION, d'où je lance requestPeers(), qui déclenche onPeersAvailable(). 
 * Je suis obligé de passer par ces deux callbacks
 *  
 * Mon feedback de primo-connexion repose sur l'intent WIFI_P2P_CONNECTION_CHANGED_ACTION -> l'extra EXTRA_WIFI_P2P_INFO contient
 * la boolean groupFormed. Si True je passe le témoin visuel (couleur label) à BLUE, si je reçois false (ce que je Rx au retour
 * à proximité après éloignement physique du GO) je passe à LTGRAY
 * 
 * ###Deuxième phase (Septembre 2022): Information connexion status inconnue ==> gros moments de solitude quand je suis sur site
 * 
 * Lifecycle Android; je souhaite avoir  l'info dans on_resume()
 * 
 * Problème: En vérifiant avec une variable boolean si c'est la première fois que je passe dans on_resume(), si je switche d'appli (hello_video) pour voir le retour
 * video, au retour ici la boolean first time a été reset, et donc bien que la connexion soit toujours là je suppose (comment le savoir???), l'appli a été redémarrée.
 * 
 * Pour être capable de vérifier le connexion statue j'utilise le NUC comme peer
 * --> connexion Android - NUC: howto dans le LOG_p2p_wpa_supplicant
 * 
 * -Hypothèse 1: android.net.wifi.p2p.WifiP2pDevice.
 * Au on_resume: le WifiP2pDevice est null la première fois, ensuite il est toujours non NULL et = à 3 (Available) quelle que soit la connexion
 * --> Donc pas informatif

 * en adb shell: dumpsys wifip2p
 * 
 * -Hypothèse 2: avoir des infos en demandant au system service WifiP2pManager
 * https://developer.android.com/reference/android/net/wifi/p2p/WifiP2pManager --> methodes
 * 
 * 2.1 - WifiP2pManager.requestGroupInfo() je ne recois qu'une liste vide mais ce serait normal puisque je ne suis pas le Group Owner
 *  
 * 2.2 - WifiP2pManager.requestConnectionInfo()
 * -J'ai l'info immédiatement. 
 * 
 * Avant première connexion j'ai: 
 * 		"groupFormed: false isGroupOwner: false groupOwnerAddress: null"
 * Après connexion, avec vérification connexion par socket send msg j'ai:
 * 		"groupFormed: true isGroupOwner: false groupOwnerAddress: /192.168.49.1"
 * Après déconnexion par le NUC avec wpa_cli -i p2p-dev-wlan0 p2p_group_remove `ls /sys/class/net/ | grep p2p` --> j'ai:
 * 		"groupFormed: false isGroupOwner: false groupOwnerAddress: null"
 * 
 * 
 * --> donc le retour de requestConnectionInfo() semble informatif sur le status de la connexion
 * 
 * */


package vvnx.P2P_wifi;

import android.app.Activity;
import android.os.Bundle;

import android.view.WindowManager;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.graphics.Color;

import android.content.Context;
import android.content.BroadcastReceiver;
import android.content.IntentFilter;
import android.net.wifi.p2p.WifiP2pConfig;
import android.net.wifi.p2p.WifiP2pDevice;
import android.net.wifi.p2p.WifiP2pManager;
import android.net.wifi.p2p.WifiP2pManager.ActionListener;
import android.net.wifi.p2p.WifiP2pManager.Channel;
import android.net.wifi.p2p.WifiP2pDeviceList;
import android.net.wifi.p2p.WifiP2pManager.PeerListListener;
import android.net.wifi.p2p.WifiP2pGroup;
import android.net.wifi.p2p.WifiP2pInfo;
import android.net.wifi.WpsInfo;

import java.net.Socket;
import java.io.PrintWriter;
import java.io.IOException;

import android.util.Log;
import android.widget.Toast;

//DateTime
import java.time.ZoneId;
import java.time.ZonedDateTime;
import java.time.format.DateTimeFormatter;




public class P2P_wifi extends Activity implements PeerListListener {

        private Button btn_1;
        public TextView txt_conn;
        private WifiP2pManager manager;
        private Channel channel;
        private WifiP2pConfig config;
        private WifiP2pDevice leRaspberry;
        boolean first_time; //default = false
        
		public static String TAG = "vvnx";
		
		private final IntentFilter intentFilter = new IntentFilter();
		private BroadcastReceiver receiver = null;
		
		

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        View view = getLayoutInflater().inflate(R.layout.P2P_wifi, null);
        setContentView(view);
        
        btn_1 = findViewById(R.id.btn_1);
        txt_conn = findViewById(R.id.txt_conn);
        txt_conn.setTextColor(Color.LTGRAY);
        
        intentFilter.addAction(WifiP2pManager.WIFI_P2P_STATE_CHANGED_ACTION);
        intentFilter.addAction(WifiP2pManager.WIFI_P2P_PEERS_CHANGED_ACTION);
        intentFilter.addAction(WifiP2pManager.WIFI_P2P_CONNECTION_CHANGED_ACTION);
        intentFilter.addAction(WifiP2pManager.WIFI_P2P_THIS_DEVICE_CHANGED_ACTION);

        
        manager = (WifiP2pManager) getSystemService(Context.WIFI_P2P_SERVICE);
        channel = manager.initialize(this, getMainLooper(), null);
        
    }
    
    
	//On y passe au launch, et à chaque réaffichage. je mets le discoverPeers ici
    @Override
    public void onResume() {
        super.onResume();
        Log.d(TAG, "onResume"); 
        
        
        //register broadcast receiver, seulement si c'est la première fois qu'on passe ici, ce serait peut être mieux de checker si le receiver est null?      
        if ( ! first_time) { 
			Log.d(TAG, "boolean first_time = default (false) donc premier passage dans onResume(), on crée un receiver et on le register");
			receiver = new P2pBroadcastReceiver(manager, channel, this);
			registerReceiver(receiver, intentFilter); 
			first_time = true;		
		} else {
			Log.d(TAG, "boolean first_time = true donc au moins deuxième passage dans onResume()");
		}
		
 
		
		//check connexion status
		manager.requestConnectionInfo(channel, new WifiP2pManager.ConnectionInfoListener() {
				@Override
				public void onConnectionInfoAvailable(WifiP2pInfo info) {
					Log.d(TAG, "onResume(): onConnectionInfoAvailable, wifip2pinfo: " + info.toString()); 
					 
					 //Yes connexion, do nothing but set le lable en BLUE 
					 if ( info.groupFormed) { 
						Log.d(TAG, "onResume(): boolean info.groupFormed = true donc on a déjà une connexion"); 
						txt_conn.setTextColor(Color.BLUE);
						} 
						
						else 
						
						
						//No connexion, set le label en GRAY et lancer discoverPeers()
						{
						Log.d(TAG, "onResume(): boolean info.groupFormed = false donc pas de connexion, on lance discoverPeers()");
						txt_conn.setTextColor(Color.LTGRAY);
						manager.discoverPeers(channel, new WifiP2pManager.ActionListener() {
							@Override
							public void onSuccess() {}
							@Override
							public void onFailure(int reasonCode) {}
						});	
						
						
						
						 
						}				
					}
			});	
		
		
		
		

		
		 
        

        
    }

    @Override
    public void onPause() {
        super.onPause();
        unregisterReceiver(receiver);
    }
    
    
    /**onPeersAvailable() appelée parce que cette activity implements PeerListListener, en réponse à 
    * ***manager.requestPeers(channel, activity);*** dans le BroadcastReceiver 
    * */
	@Override
    public void onPeersAvailable(WifiP2pDeviceList peerList) {
        //sans android.permission.ACCESS_FINE_LOCATION au runtime (pas manifest only), la WifiP2pDeviceList est vide
        //Log.d(TAG, "onPeersAvailable dans P2P_wifi activity et la liste = " + peerList.toString());//plusieurs infos pas seulement MACADDR
        
        /**
         * Au début je récupérais par mac addr, et il fallait que je vérifie si null ET le status:
         * WifiP2pDevice monPeerDevice = peerList.get("ba:27:eb:92:fc:8f"); //zero
         * if (monPeerDevice != null && monPeerDevice.status == 3) { //
         * 
         * Depuis mars 2022 j'iterate avec getDeviceList() dans la peerList et je recup par deviceName
         * Pour attribuer le name côté linux c'est: wpa_supplicant.conf: syntaxe: device_name=Zero
         * 
         * */
		
        for (WifiP2pDevice unPeer : peerList.getDeviceList()) {			
			//on vient de trouver un device avec nom=zero
			//sans check du status (3 = AVAILABLE) je passe 200x/s ici (et donc dans manager.connect()) et le manager a pas l'air d'aimer 
			//doc status: https://developer.android.com/reference/android/net/wifi/p2p/WifiP2pDevice#constants_1
			//if (unPeer.deviceName.equals("Zero") && unPeer.status == 3) {
			if ((unPeer.deviceName.equals("NUC") || unPeer.deviceName.equals("Zero")) && unPeer.status == 3) {	
				Log.d(TAG, "Dans la peerList on a un Peer dont name matche avec status = " + unPeer.status);
				
				if (leRaspberry == null) {
					Log.d(TAG, "dans onPeersAvailable(): première fois qu on voit le raspberry, on configure connexion + Connect()");
					leRaspberry = unPeer;
					config = new WifiP2pConfig();
					config.deviceAddress = leRaspberry.deviceAddress;
					config.wps.setup = WpsInfo.PBC;
					} 
				
				Connect();				
				}		
		}
 
    }
    
    public void Connect() {
		Log.d(TAG, "On lance un manager.connect()");
		manager.connect(channel, config, new ActionListener() {
		            @Override
		            public void onSuccess() { //pas informatif sur le statut de la connexion
						//Log.d(TAG, "connect() --> onSuccess");
		                //Toast.makeText(P2P_wifi.this, "connect() --> onSuccess", Toast.LENGTH_SHORT).show();
		            }
		
		            @Override
		            public void onFailure(int reason) { //pas informatif sur le statut de la connexion
						//Log.d(TAG, "connect() --> onFailure, reason: " + reason);
						//Toast.makeText(P2P_wifi.this, "connect() --> onFailure, reason: " + reason, Toast.LENGTH_SHORT).show();
		            }
				});	
	}
    
    
    /**bouton pour tests envoi messages (datetime) sur Socket, Rx avec:
    socat TCP-LISTEN:4696,fork EXEC:/root/myscript.sh & (dans le sysinit, voir RPiMonActivity.java)
    ou pour des tests, plus simple:
    socat TCP-LISTEN:4696,fork -
    * */
    
    public void ActionPressBouton_1(View v) {
		
		//Récup DateTime
		ZonedDateTime zdt = ZonedDateTime.now(ZoneId.of("Europe/Paris"));
		String maTimePattern = "dd/MM HH:mm:ss"; //https://developer.android.com/reference/java/time/format/DateTimeFormatter#patterns 
		DateTimeFormatter dtf = DateTimeFormatter.ofPattern(maTimePattern);
		String maDateTime = zdt.format(dtf);
		
		Log.d(TAG, "press bouton, datetime = " + maDateTime);
		
		//Envoi via socket sur une IP, dans un Thread sinon android.os.NetworkOnMainThreadException
		new Thread(new Runnable(){
		@Override
			public void run() {	        
				try {
					Socket socket = new Socket("192.168.49.1", 4696);
			        PrintWriter writer = new PrintWriter(socket.getOutputStream(), true);
			        writer.println(maDateTime);
			        socket.close(); //sinon accumulation de connexions peut poser pb socat (address already in use) mais aux tests mainly
				} catch (IOException e) {
                    Log.d(TAG, "erreur send socket :" + e.getMessage());
                } 
            }
		}).start();	
	}
}

