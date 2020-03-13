package source;

import java.net.ServerSocket;
import java.net.Socket;

/**
 * MyThread
 */
public class MyThread extends Thread{
    private ServerSocket server_socket;

    MyThread( ServerSocket sc){
        server_socket = sc;
    }
    
    public void run() {
        try {
            server_socket.accept();
            
        } catch (Exception e) {
            //TODO: handle exception
        }
    }
}