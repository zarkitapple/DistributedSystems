package src.clientjava;

import java.net.Socket;
import java.net.ServerSocket;
import java.net.UnknownHostException;
import java.io.*;

/**
 * This class represents the client-server thread to listen for incomning
 * requests from other clients 
 */
public class ServerThread extends Thread {
    /* Server socket */
    private ServerSocket server_socket;
    /* User name used in case of automatic disconnect */
    byte [] user_name;
    /* Boolean flag used to stop the run mehtod of the thread as it is in a loop*/
    boolean running;

     /* Constructor */
    ServerThread(ServerSocket sc,byte [] name) {
        server_socket = sc;
        user_name = name;
        running = true;
    }
    /**
     * Close the server socket 
     */
    public void closeConnection () {
        try {
            server_socket.close();
            running = false;
        } catch (IOException e) {
           /* Do nothing as the exception will aleays be thrown when closing a 
              ServerSocket on accept*/
        }
       
    }
    /** 
     * Close server connection from client side on quit 
     */
    public void closeConnectionQuit (String server, int port) {
        
        /* Link client-server in server side creating in and out streams for data */
        /* Try-resources */
        try (   Socket socket = new Socket(server,port);
                DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
                DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());) 
        {
            /* Send command */
            send_stream.write(("DISCONNECT"+'\0').getBytes());
			send_stream.flush();

            /* Send name of user quitting */
			send_stream.write(user_name);
            send_stream.flush();
            
            receive_Stream.readInt();
            
            /*Close the socket connecting to the server */
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
    /**
     * Listen for incoming connnections in a loop, controlled by running flag
     */
    public void run() {
        while (running){
            try {
                /* Listen for incomming connections */
                Socket socket = server_socket.accept();
                /* New connection, handle the request by creating a new thread */
                new ManageRequest(socket).start();

            } catch (IOException e) {
                /* Do nothing as the exception will aleays be thrown  when closing a 
              ServerSocket on accept*/
            }
        }
    }
}