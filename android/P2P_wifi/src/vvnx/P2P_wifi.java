/*
 * 
 * 
adb uninstall vvnx.P2P_wifi && \
adb install out/target/product/generic_arm64/system/app/P2P_wifi/P2P_wifi.apk && \
adb shell pm grant vvnx.P2P_wifi android.permission.ACCESS_FINE_LOCATION
 * 
 * 
 * le point de départ de la connexion = discoverPeers(). Ensuite le broadcastReceiver reçoit
 * WIFI_P2P_PEERS_CHANGED_ACTION, d'où je lance requestPeers(), qui déclenche onPeersAvailable(). 
 * Je suis obligé de passer par ces deux callbacks parce que mes essais (peu nombreux certes) de 
 * connecter directement sur une MAC ADDRESS n'ont pas marché, et c'est cohérent avec ce que j'ai lu.
 * 
 * Mon feedback de connexion repose sur l'intent WIFI_P2P_CONNECTION_CHANGED_ACTION -> l'extra EXTRA_WIFI_P2P_INFO contient
 * la boolean groupFormed. Si True je passe le témoin visuel (couleur label) à BLUE, si je reçois false (ce que je Rx au retour
 * à proximité après éloignement physique du GO) je passe à LTGRAY
 * 
 * Pour P2P côté linux voir LOG_p2p_wpa_supplicant
 * 
 * */


package vvnx.P2P_wifi;

import android.app.Activity;
import android.os.Bundle;

import android.view.WindowManager;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

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
        receiver = new P2pBroadcastReceiver(manager, channel, this);
        registerReceiver(receiver, intentFilter);
        
        Log.d(TAG, "onResume"); 
        manager.discoverPeers(channel, new WifiP2pManager.ActionListener() {
                    @Override
                    public void onSuccess() {
                        //Toast.makeText(P2P_wifi.this, "Discovery Initiated", Toast.LENGTH_SHORT).show();
                    }

                    @Override
                    public void onFailure(int reasonCode) {
                        //Toast.makeText(P2P_wifi.this, "Discovery Failed : " + reasonCode, Toast.LENGTH_SHORT).show();
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
         * if (monPeerDevice != null && monPeerDevice.status == 3) { //sans check du status je passe 200x/s ici (et donc dans manager.connect()) et ça fait planter connexion
         * 
         * Depuis mars 2022 je cherche le peer par deviceName dans la WifiP2pDeviceList que j'iterate avec getDeviceList().
         * Pour attribuer le name côté linux c'est: wpa_supplicant.conf: syntaxe: device_name=Zero
         * 
         * */
		
        for (WifiP2pDevice monPeerDevice : peerList.getDeviceList()) {			
			//Log.d(TAG, "name:" + monPeerDevice.deviceName);
			
			if (monPeerDevice.deviceName.equals("Zero")) {
			config = new WifiP2pConfig();
			config.deviceAddress = monPeerDevice.deviceAddress;
			config.wps.setup = WpsInfo.PBC;
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
		}
 
    }
    
    
    //bouton pour tests envoi message sur Socket, Rx avec #socat TCP-LISTEN:5778,fork -
    public void ActionPressBouton_1(View v) {
		
		Log.d(TAG, "press bouton");
		//DateTime
		ZonedDateTime zdt = ZonedDateTime.now(ZoneId.of("Europe/Paris"));
		String maTimePattern = "dd/MM HH:mm:ss"; //https://developer.android.com/reference/java/time/format/DateTimeFormatter#patterns 
		DateTimeFormatter dtf = DateTimeFormatter.ofPattern(maTimePattern);
		String maDateTime = zdt.format(dtf);
		Log.d(TAG, "datetime = " + maDateTime);
		
		
		
		//Thread sinon android.os.NetworkOnMainThreadException
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

