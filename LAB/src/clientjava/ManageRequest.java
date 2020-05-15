package src.clientjava;

import java.net.Socket;
import java.io.*;

public class ManageRequest extends Thread {
    private final String files_path = "./UserSharedFiles/";
    private final String get_file = "GET_FILE";
    private Socket socket;

    ManageRequest(Socket sc){
        socket = sc;
    }
    public void run() {

        try ( BufferedReader receive_data = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                DataOutputStream response = new DataOutputStream(socket.getOutputStream());) 
        {
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
                else {
                    output = 1;
                }
            }
            else {
                output = 2;
            }

            response.writeInt(output);

            if (output == 0){
                try (   BufferedReader bufferedReader = new BufferedReader(new FileReader(file_path));
                        BufferedWriter bufferedWriter = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));) 
                {
                    String line;

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