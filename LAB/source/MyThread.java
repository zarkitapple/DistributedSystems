package source;

import java.net.Socket;
import java.net.ServerSocket;
import java.io.*;

/**
 * MyThread
 */
public class MyThread extends Thread {
    private ServerSocket server_socket;


    MyThread(ServerSocket sc) {
        server_socket = sc;
    }
    public void closeConnectionManually () {
        try {
            server_socket.close();
        } catch (Exception e) {
            System.err.println(e.toString());
        }
       
    }
    public void closeConnectionAuto (String server, int port, byte [] user) {
        try {
            Socket socket = new Socket(server,port);
			
			DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
			DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());


			send_stream.write(("DISCONNECT"+'\0').getBytes());
			send_stream.flush();

			send_stream.write(user);
            send_stream.flush();
            
            int receive_output = receive_Stream.readInt();
 
            send_stream.close();
			receive_Stream.close();
            socket.close();
            
            server_socket.close();
        } catch (Exception e) {
            System.err.println(e.toString());

        }
       
    }
    public void run() {
        Socket socket;
        DataInputStream receive_Stream;
        DataOutputStream send_stream;


        try {
            /* Needs to be made multithreade */
                socket = server_socket.accept();
                System.out.println("Inside thread 2 ");
                receive_Stream = new DataInputStream(socket.getInputStream());
                System.out.println("Inside thread");
                send_stream = new DataOutputStream(socket.getOutputStream());
                System.out.println("Inside thread");

                receive_Stream.close();
                send_stream.close();
                socket.close();

        } catch (Exception e) {
            System.err.println(e.toString());
        }

    }
}