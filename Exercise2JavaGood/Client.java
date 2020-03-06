/**
 * client TCP stream socket
 */

import java.io.*;
import java.net.*;


public class Client {
    private static final int BUFFER_SIZE = 256;
    public static void main(String[] args) {

        if(args.length !=1){
            System.out.println("You must enter the server ip");
            System.exit(0);
        }
        try {
            String host = args[0];
            Socket socket = new Socket(host,4200);

            DataOutputStream send_Stream = new DataOutputStream(socket.getOutputStream());
            BufferedReader terminal_Stream= new BufferedReader(new InputStreamReader(System.in));
            DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());
            String message = "";
            while (true) {
                message = terminal_Stream.readLine();
                send_Stream.write(message.getBytes());
                if(message.equals("EXIT")){
                    break;
                }
                byte[] aux = new byte[BUFFER_SIZE];
                receive_Stream.read(aux);
                String s = new String(aux);
                System.out.println(s);
            }
            socket.close();
            send_Stream.close();
            receive_Stream.close();

        } catch (Exception e) {
            System.err.println("Exeception "+ e.toString());
            e.printStackTrace();
        }
    }
}