package vvnx.P2P_wifi;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.NetworkInfo;
import android.net.wifi.p2p.WifiP2pInfo;
import android.net.wifi.p2p.WifiP2pDevice;
import android.net.wifi.p2p.WifiP2pManager;
import android.net.wifi.p2p.WifiP2pManager.Channel;
import android.net.wifi.p2p.WifiP2pManager.PeerListListener;
import android.util.Log;
import android.graphics.Color;


public class P2pBroadcastReceiver extends BroadcastReceiver {

    private WifiP2pManager manager;
    private Channel channel;
    private P2P_wifi activity;

    /**
     * @param manager WifiP2pManager system service
     * @param channel Wifi p2p channel
     * @param activity activity associated with the receiver
     */
    public P2pBroadcastReceiver(WifiP2pManager manager, Channel channel, P2P_wifi activity) {
        super();
        this.manager = manager;
        this.channel = channel;
        this.activity = activity;
    }

    /*
     * (non-Javadoc)
     * @see android.content.BroadcastReceiver#onReceive(android.content.Context,
     * android.content.Intent)
     */
    @Override
    public void onReceive(Context context, Intent intent) {
        String action = intent.getAction();
        if (WifiP2pManager.WIFI_P2P_STATE_CHANGED_ACTION.equals(action)) {
            Log.d(P2P_wifi.TAG, "onReceive: P2P state changed");
        } else if (WifiP2pManager.WIFI_P2P_PEERS_CHANGED_ACTION.equals(action)) {
            Log.d(P2P_wifi.TAG, "onReceive: P2P peers changed");
           
            /**Mon activité principale implements PeerListListener et override donc une methode onPeersAvailable()
             * qui va me permettre de récupérer la liste des peers
             * 
             * */
            manager.requestPeers(channel, activity);
            
        } else if (WifiP2pManager.WIFI_P2P_CONNECTION_CHANGED_ACTION.equals(action)) {
			//Toast.makeText(activity, "WIFI_P2P_CONNECTION_CHANGED_ACTION", Toast.LENGTH_SHORT).show();
			final WifiP2pInfo wifiP2pInfo = (WifiP2pInfo)intent.getParcelableExtra(WifiP2pManager.EXTRA_WIFI_P2P_INFO);
			Log.d(P2P_wifi.TAG, "onReceive: P2P connection changed, wifip2pinfo: " + wifiP2pInfo.toString());
			/**
			 * Quand connexion OK je Rx:
			 * wifiP2pInfo.toString() --> onReceive: wifip2pinfo: groupFormed: true isGroupOwner: false groupOwnerAddress: /192.168.49.1
			 * */
			if (wifiP2pInfo.groupFormed) {
				Log.d(P2P_wifi.TAG, "wifip2pinfo.groupFormed = true");
				activity.txt_conn.setTextColor(Color.BLUE);				
			} else { //
				Log.d(P2P_wifi.TAG, "wifip2pinfo.groupFormed = false");
				activity.txt_conn.setTextColor(Color.LTGRAY);	
				}
			
			
        } else if (WifiP2pManager.WIFI_P2P_THIS_DEVICE_CHANGED_ACTION.equals(action)) {
			Log.d(P2P_wifi.TAG, "onReceive: P2P this device changed");
        }
    }
}
