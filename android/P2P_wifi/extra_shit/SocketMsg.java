/**
 * Java hors Android sockets
 * 
 * export JAVA_HOME=/initrd/mnt/dev_save/packages/jdk-11
 * export PATH=/initrd/mnt/dev_save/packages/jdk-11/bin:$PATH
 * javac SocketMsg.java
 * 
 * 
 * 
 * 
 * 
 * 
 *  socat TCP-LISTEN:5778,fork -
 *netstat -ntp | grep 5778
* 
* */


import java.io.*;
import java.net.*;

public class SocketMsg {

    public static void main(String[] args) {	
		
 	String hostName = "127.0.0.1";
	int portNumber = 5778;       
    
    try {
    Socket echoSocket = new Socket(hostName, portNumber);
    PrintWriter out = new PrintWriter(echoSocket.getOutputStream(), true);
    out.println("foobar");
    out.close();
	echoSocket.close();     
	} catch (UnknownHostException e) {
	} catch (IOException e) { }
	

       
    }

}
