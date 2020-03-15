package source;
import java.net.Socket;
import java.util.concurrent.atomic.AtomicBoolean;
import java.net.ServerSocket;
import java.io.*;
/**
 * MyThread
 */
public class MyThread extends Thread{
    private ServerSocket server_socket;
    private AtomicBoolean exit_flag;

    MyThread( ServerSocket sc,AtomicBoolean flag){
        server_socket = sc;
        exit_flag = flag;
    }
    
    public void run() {
        Socket socket;
        DataInputStream receive_Stream;
        DataOutputStream send_stream;
            System.out.println("Inside thread");
            while(exit_flag.get()){
                try {
                    server_socket.setSoTimeout(1);
                    //System.out.println("BEFORE ACCEPT");
                    socket = server_socket.accept();
                    //System.out.println("AFTER ACCEPT");
                    /* Needs to be made multithreade */
                    if(socket!=null){
                        socket = server_socket.accept();
                        System.out.println("Inside thread");
                        receive_Stream = new DataInputStream(socket.getInputStream());
                        System.out.println("Inside thread");
                        send_stream = new DataOutputStream(socket.getOutputStream());
                        System.out.println("Inside thread");
            
                        receive_Stream.close();
			            send_stream.close();
			            socket.close();
                    }
                } catch (Exception e) {
                    //System.out.println("Inside execption");
                }
                
            }
            
    }
}