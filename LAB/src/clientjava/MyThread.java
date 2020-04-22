package src.clientjava;

import src.clientjava.UsersData;
import java.net.Socket;
import java.util.concurrent.ConcurrentHashMap;
import java.net.ServerSocket;
import java.io.*;

/**
 * MyThread
 */
public class MyThread extends Thread {


    public class ManageRequest extends Thread {
        private final String files_path = "./UserFiles";
        private final String get_file = "GET_FILE";
        private Socket socket;

        ManageRequest(Socket sc){
            socket = sc;
        }
        public void run() {
            try {

                BufferedReader receive_data = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                String operation = receive_data.readLine();

                String file_path = null;
                int output;

                if(operation.equals(get_file)){
                    String file_name = receive_data.readLine();

                    file_path = files_path.concat(file_name);
                    File file = new File (file_path);
                    if(file.exists()){
                        output =  0;
                    }
                    output = 1;
                }
                else {
                    output = 2;
                }

                DataOutputStream response = new DataOutputStream(socket.getOutputStream());
                response.writeInt(output);

                if(output == 0){
                    
                    BufferedReader bufferedReader = new BufferedReader(new FileReader(file_path));
                    BufferedWriter bufferedWriter = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
                    String line;

                    while((line = bufferedReader.readLine()) != null){
                        bufferedWriter.write(line);
                        bufferedWriter.flush();
                    }
                    bufferedReader.close();
                    bufferedWriter.close();
                }

                receive_data.close();
                response.close();

                socket.close();

            } catch (Exception e) {
                System.err.println(e.toString());
            }
            
            

        }
    }
    ConcurrentHashMap <String, UsersData> users_data;
    private ServerSocket server_socket;
    byte [] user_name;

    MyThread(ServerSocket sc,byte [] name,ConcurrentHashMap<String, UsersData> map) {
        server_socket = sc;
        user_name = name;
        users_data = map;
    }
    public void closeConnectionManually () {
        try {
            server_socket.close();
        } catch (Exception e) {
            System.err.println(e.toString());
        }
       
    }
    public void closeConnectionAuto (String server, int port) {
        try {
            Socket socket = new Socket(server,port);
			
			DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
			DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());


			send_stream.write(("DISCONNECT"+'\0').getBytes());
			send_stream.flush();

			send_stream.write(user_name);
            send_stream.flush();
            
            receive_Stream.readInt();
 
            send_stream.close();
			receive_Stream.close();
            socket.close();
            
            server_socket.close();
        } catch (Exception e) {
            System.err.println(e.toString());
        }
       
    }
    public void run() {
    while (true){
        try {
            /* Needs to be made multithreade */
            Socket socket = server_socket.accept();

            new ManageRequest(socket).start();

        } catch (Exception e) {
            System.err.println(e.toString());
        }
    }

    }
}