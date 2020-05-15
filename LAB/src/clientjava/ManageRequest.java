package src.clientjava;

import java.net.Socket;
import java.io.*;

/**
 * This class manages a get_file request 
 */
public class ManageRequest extends Thread {
    /* Path where the user files to be accessed remotely are stored */
    private final String files_path = "./UserSharedFiles/";
    /* Name of the get_file command */
    private final String get_file = "GET_FILE";
    /* Reference to the Socket of the client-server */
    private Socket socket;

     /* Constructor */
    ManageRequest(Socket sc){
        socket = sc;
    }
    
    public void run() {
        /* Open output and input streams to manage the get_file request*/
		/* Try-resources */
        try ( BufferedReader receive_data = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                DataOutputStream response = new DataOutputStream(socket.getOutputStream());) 
        {
            /* read the name of the operation */
            String operation = receive_data.readLine();
            String file_path = null;

            int output;
            if(operation.equals(get_file)){
                /* Read name of the file to be sent */
                String file_name = receive_data.readLine();
                /* Construct the path of the file */
                file_path = files_path.concat(file_name);
                File file = new File (file_path);
                /* Check if file exists */
                if(file.exists()){
                    output =  0;
                }
                else {
                    output = 1;
                }
            }
            else {
                output = 2;
            }
            /* Send the response to the requesting client */
            response.writeInt(output);

            /* if success */
            if (output == 0){
                 /* Open input streamt to read file and output stream to send file*/
                /* Try-resources */
                try (   BufferedReader bufferedReader = new BufferedReader(new FileReader(file_path));
                        BufferedWriter bufferedWriter = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));) 
                {
                    String line;
                    /* Read line by line the file and send it to the client */
                    while((line = bufferedReader.readLine()) != null){
                    
                    bufferedWriter.write(line);
                    bufferedWriter.flush();
                    }
                } catch (IOException e) {
                    System.err.println(e.toString());
                }

            }

        } catch (NullPointerException e) {
            System.err.println(e.toString());
        } catch (IOException e) {
            System.err.println(e.toString());
        } catch (SecurityException e) {
            System.err.println(e.toString());
        }
    }
}