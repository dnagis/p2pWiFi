/*
 * adb uninstall vvnx.P2P_wifi
 * adb install out/target/product/generic_arm64/system/app/P2P_wifi/P2P_wifi.apk
 * 
 * adb shell pm grant vvnx.P2P_wifi android.permission.ACCESS_FINE_LOCATION
 * 
 * 
 * le point de départ de la connexion = discoverPeers(). Ensuite le broadcastReceiver reçoit
 * WIFI_P2P_PEERS_CHANGED_ACTION, d'où je lance requestPeers(), qui déclenche onPeersAvailable(). 
 * Je suis obligé de passer par ces deux callbacks parce que mes essais (peu nombreux certes) de 
 * connecter directement sur une MAC ADDRESS n'ont pas marché, et c'est cohérent avec ce que j'ai lu.
 * 
 * 
 * côté linux:
 * wpa_cli -i p2p-dev-wlan0 p2p_group_add persistent
 * ifconfig `ls /sys/class/net/ | grep p2p` 192.168.49.1
 * /etc/udhcpd.conf --> adapter le nom de l'interface
 * udhcpd -f /etc/udhcpd.conf
 * wpa_cli -i `ls /sys/class/net/ | grep p2p` wps_pbc
 * 
 * */


package vvnx.P2P_wifi;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;
import android.content.Context;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.IntentFilter;
import android.net.wifi.p2p.WifiP2pConfig;
import android.net.wifi.p2p.WifiP2pDevice;
import android.net.wifi.p2p.WifiP2pManager;
import android.net.wifi.p2p.WifiP2pManager.ActionListener;
import android.net.wifi.p2p.WifiP2pManager.Channel;
import android.net.wifi.p2p.WifiP2pDeviceList;
import android.net.wifi.p2p.WifiP2pManager.PeerListListener;
import android.net.wifi.WpsInfo;
import android.util.Log;
import android.widget.Toast;




public class P2P_wifi extends Activity implements PeerListListener {

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
        
        WifiP2pDevice monPeerDevice = peerList.get("98:af:65:ce:18:6f"); //NUC10i7 principal
        //WifiP2pDevice monPeerDevice = peerList.get("ba:27:eb:92:fc:8f"); //zero
        
        if (monPeerDevice != null && monPeerDevice.status == 3) { //le check du status m'évite de passer 200x/s ici (et donc dans manager.connect()
			Log.d(TAG, "WifiP2pDevice n'est pas null, son status=" + monPeerDevice.status); //au départ: 3

			config = new WifiP2pConfig();
			config.deviceAddress = monPeerDevice.deviceAddress;
			config.wps.setup = WpsInfo.PBC;
			manager.connect(channel, config, new ActionListener() {
	            @Override
	            public void onSuccess() { //pas informatif
					//Log.d(TAG, "connect() --> onSuccess");
	                //Toast.makeText(P2P_wifi.this, "connect() --> onSuccess", Toast.LENGTH_SHORT).show();
	            }
	
	            @Override
	            public void onFailure(int reason) { //pas informatif
					//Log.d(TAG, "connect() --> onFailure, reason: " + reason);
					//Toast.makeText(P2P_wifi.this, "connect() --> onFailure, reason: " + reason, Toast.LENGTH_SHORT).show();
	            }
			});
		}   
    }
 
}

