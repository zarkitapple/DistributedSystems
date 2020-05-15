package src.clientjava;

import java.net.Socket;
import java.net.ServerSocket;
import java.net.UnknownHostException;
import java.io.*;

/**
 * MyThread
 */
public class ServerThread extends Thread {
    private ServerSocket server_socket;
    byte [] user_name;
    boolean running;

    ServerThread(ServerSocket sc,byte [] name) {
        server_socket = sc;
        user_name = name;
        running = true;
    }
    public void closeConnection () {
        try {
            server_socket.close();
            running = false;
        } catch (IOException e) {
           /* Do nothing as the exception will aleays be thrown  when closing a 
              ServerSocket on accept*/
        }
       
    }
    public void closeConnectionQuit (String server, int port) {

        try (   Socket socket = new Socket(server,port);
                DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
                DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());) 
        {
            send_stream.write(("DISCONNECT"+'\0').getBytes());
			send_stream.flush();

			send_stream.write(user_name);
            send_stream.flush();
            
            receive_Stream.readInt();

            server_socket.close();
            running = false;

        } catch (UnknownHostException e) {
			System.err.println(e.toString());
		} catch (EOFException e) {
			System.err.println(e.toString());
		} catch (IOException e) {
			System.err.println(e.toString());
		} catch (SecurityException e){
			System.err.println(e.toString());
        } 
        
    }
    public void run() {
        while (running){
            try {
                Socket socket = server_socket.accept();

                new ManageRequest(socket).start();

            } catch (IOException e) {
                /* Do nothing as the exception will aleays be thrown  when closing a 
              ServerSocket on accept*/
            }
        }
    }
}