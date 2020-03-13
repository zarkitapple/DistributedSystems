package source;

import java.io.DataInputStream;
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
            Socket socket = server_socket.accept();
            DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());
            
            
        } catch (Exception e) {
            System.err.println("Exception "+e.toString());
            e.printStackTrace();
        }
    }
}