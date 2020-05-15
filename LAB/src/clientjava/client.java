package src.clientjava;

import src.clientjava.touppercase.ToUpperCaseServiceService;
import src.clientjava.touppercase.ToUpperCaseService;
import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.HashMap;


import gnu.getopt.Getopt;

class client {
	
	
	/******************* ATTRIBUTES *******************/
	
	private static String _server = null;
	private static int _port = -1;
	/* Maximum message size*/
	private static final int message_size = 256;
	/* Variable to check ff the calling user is connected*/
	private static boolean connected = false;
	/* Reference to the client-server thread */
	private static ServerThread server_Thread = null;
	/* Variable to hold the name of the calling connected user*/
	private static String connected_user_name = null;
	/* Hash map used to store the ip and port of the connected users to the server*/
	private static HashMap <String, UsersData > users_data = null;
	/* Path where the user files from get_file will be stored */ 
	private static String file_path = "./UserFiles/";  

	/* Command enum of different commands sent to the server */
	enum COMMANDS {
		REGISTER,
		UNREGISTER,
		CONNECT,
		DISCONNECT,
		PUBLISH,
		DELETE,
		LIST_USERS,
		LIST_CONTENT,
		GET_FILE
	}
	
	/***************************** AUXILIARY ******************************/

	/**
	 * Transform a string to a byte array and add the end of string character
	 * if the string is > 255 characters it will be truncated
	 * @param userString
	 * @return byte_array of the input string
	 */
	private static byte [] parse_string (String userString) {
		byte [] byte_array;
		/* Truncate if larger than 255 */
		if (userString.length()>message_size-1){
			byte_array = userString.substring(0, message_size).getBytes();
			/* Add end of string character */
			byte_array[message_size-1] = '\0';
		}
		/* Add end of string character */
		else {
			userString = userString +'\0';
			byte_array = userString.getBytes();
		}
		return byte_array;
	}
	/**
	 * Transform the commmand string to a byte array and add end of line character
	 * @param type
	 * @return byte array of command
	 */
	private static byte [] parse_command (String type){
		String command_string = COMMANDS.valueOf(type).toString();
		command_string = command_string + '\0';
		byte [] command = command_string.getBytes();
		return command;
	}
	/**
	 * Create the data structure hash map and the directories needed to store files
	 * @return 0 if no error, 1 if error
	*/
	private static int init_client(){
		
		/* Create the hash map to store the ip and ports of the user */
		users_data = new HashMap<String, UsersData>();
		/* Check if the directories have already been created to not overwrite */
		if(new File("./UserSharedFiles").exists() && new File("./UserFiles").exists()){
			return 0;
		}
		/* If the directories have not been created,create them */
		if (!(new File ("./UserSharedFiles").mkdirs())){
			System.err.println("ERROR WHEN CREATING UserShareFiles directory");
			return -1;
		}
		if(!(new File ("./UserFiles").mkdirs())){
			System.err.println("ERROR WHEN CREATING UserFiles directory");
			return -1;
		}
		return 0;	
	}


	/********************* REQUESTED FUNCTIONALITY ********************/
	/**
	 * Registers the user in the system
	 * @param user - User name to register in the system
	 */
	static int register(String user) 
	{	
		/* Transform the user name and commmand to byte arrays */
		byte [] user_name = parse_string(user);
		byte [] command = parse_command(COMMANDS.REGISTER.toString());
		/* Link the socket to connect with the server and send the request */
		/* Try-resources */
		try (	Socket socket = new Socket(_server,_port);
				DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
				DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());) 
		{
			/* Send command */
			send_stream.write(command);
			send_stream.flush();
			/* Send user name */
			send_stream.write(user_name);
			send_stream.flush();
			/* Receive server response */
			int receive_output = receive_Stream.readInt();

			switch (receive_output) {
				case 1:
					System.err.println("USER NAME IN USE");
					break;
				case 2:
					System.err.println("REGISTER FAIL");
					break;
				default:
					System.out.println("REGISTER OK");
			}

		} catch (UnknownHostException e) {
			System.err.println("REGISTER FAIL" + e.toString());
		} catch (EOFException e) {
			System.err.println("REGISTER FAIL" + e.toString());
		} catch (IOException e) {
			System.err.println("REGISTER FAIL" + e.toString());
		} catch (SecurityException e){
			System.err.println("REGISTER FAIL" + e.toString());
		} 
		return 0;
	}
	
	/**
	 * Unregister the user from the server 
	 * @param user - User name to unregister in the server 
	 */
	static int unregister(String user) 
	{
		/* Transform the user name and commmand to byte arrays */
		byte [] user_name = parse_string(user);
		byte [] command = parse_command(COMMANDS.UNREGISTER.toString());
		/* Link the socket to connect with the server and send the request */
		/* Try-resources */
		try (	Socket socket = new Socket(_server,_port);
				DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
				DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());) 
		{
			/* Send the command */
			send_stream.write(command);
			send_stream.flush();
			/* Send the user name */
			send_stream.write(user_name);
			send_stream.flush();
			/* Receive server response */
			int receive_output = receive_Stream.readInt();

			switch (receive_output) {
				case 1:
					System.err.println("USER DOES NOT EXIST");
					break;
				case 2:
					System.err.println("UNREGISTER FAIL");
					break;
				default:
					System.out.println("UNREGISTER OK");
			}

		} catch (UnknownHostException e) {
			System.err.println("UNREGISTER FAIL" + e.toString());
		} catch (EOFException e) {
			System.err.println("UNREGISTER FAIL" + e.toString());
		} catch (IOException e) {
			System.err.println("UNREGISTER FAIL" + e.toString());
		} catch (SecurityException e){
			System.err.println("UNREGISTER FAIL" + e.toString());
		} 
		return 0;
	}
	
    /**
	 * Connects the calling user to the server
	 * @param user - User name to connect to the system
	 */
	static int connect(String user) 
	{

		/* Check if the user is connected already */
		if(connected){
			System.err.println("CONNECT FAIL, USER ALREADY CONNECTED");
			return 0;
		}
		byte [] user_name = parse_string(user);
		byte [] command = parse_command(COMMANDS.CONNECT.toString());
		int receive_output = 3;
		/* Link the socket to connect with the server and send the request */
		/* Try-resources */
		try (	Socket socket = new Socket(_server,_port);
				DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
				DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());) 
		{
			/* Create the client-server thread with random port and start the thread */
			ServerSocket server_socket = new ServerSocket(0);
			int server_port = server_socket.getLocalPort();
			
			server_Thread = new ServerThread(server_socket,user_name);
			server_Thread.start();

			/* Send command */
			send_stream.write(command);
			send_stream.flush();

			/* Send name of user requesting the operation */
			send_stream.write(user_name);
			send_stream.flush();
			String port = Integer.toString(server_port) +"\0";
			send_stream.write(port.getBytes());
			send_stream.flush();

			/* Recieve the server output */
			receive_output = receive_Stream.readInt();

			switch (receive_output) {
				case 1:
					System.err.println("CONNECT FAIL, USER DOES NOT EXIST");
					/* An error occurred, close the client-server thread */
					server_Thread.closeConnection();
					server_Thread.join();
					server_Thread = null;
					break;
				case 2:
					System.err.println("CONNECT FAIL, USER ALREADY CONNECTED");
					/* An error occurred, close the client-server thread */
					server_Thread.closeConnection();
					server_Thread.join();
					server_Thread = null;
					break;
				case 3:
					System.err.println("CONNECT FAIL");
					/* An error occurred, close the client-server thread */
					server_Thread.closeConnection();
					server_Thread.join();
					server_Thread = null;
					break;
				default:
					System.out.println("CONNECT OK");
					/* Success set connected to true and the user name */
					connected = true;
					connected_user_name = user;
			}

		} catch (UnknownHostException e) {
			System.err.println("CONNECT FAIL" + e.toString());
		} catch (EOFException e) {
			System.err.println("CONNECT FAIL" + e.toString());
		} catch (IOException e) {
			System.err.println("CONNECT FAIL" + e.toString());
		} catch (InterruptedException e){
			System.err.println("CONNECT FAIL" + e.toString());
		} catch (SecurityException e){
			System.err.println("CONNECT FAIL" + e.toString());
		} catch (IllegalThreadStateException e ){
			System.err.println("CONNECT FAIL" + e.toString());
		} finally {
			/* If any exeception occurred then close the client-server thread */
			if(receive_output!=0){
				if(server_Thread != null){
					server_Thread.closeConnection();
					try {
						server_Thread.join();
						server_Thread = null;
					} catch (InterruptedException e){
						System.err.println(e.toString());
					}
				}
			}
		}
		return 0;
	}
	
	 /**
	 * Discconects the calling user from the server 
	 * 
	 * @param user - User name to disconnect from the system
	 */
	static int disconnect(String user) 
	{
		/* Check if the user is connected */
		if(!connected){
			System.err.println("DISCONNECT FAIL / USER NOT CONNECTED");
			return 0;
		}
		/* Check if the input user name is the same as the connected user name
			to avoid disconnects of different users*/
		if(!user.equals(connected_user_name)){
			System.err.println("DISCONNECT FAIL");
			return 0;
		}
		/* Transform the user name and command to byte arrays*/
		byte [] user_name = parse_string(user);
		byte [] command = parse_command(COMMANDS.DISCONNECT.toString());
		/* Link the socket to connect with the server and send the request */
		/* Try-resources */
		try (	Socket socket = new Socket(_server,_port);
				DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
				DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());) 
		{
			/* Send command */
			send_stream.write(command);
			send_stream.flush();
			/* Send user name */
			send_stream.write(user_name);
			send_stream.flush();
			/* receive output from server*/
			int receive_output = receive_Stream.readInt();

			switch (receive_output) {
				case 1:
					System.err.println("DISCONNECT FAIL / USER DOES NOT EXIST");
					break;
				case 2:
					System.err.println("DISCONNECT FAIL / USER NOT CONNECTED");
					break;
				case 3:
					System.err.println("DISCONNECT FAIL");
					break;
				default:
					System.out.println("DISCONNECT OK");
			}

		} catch (UnknownHostException e) {
			System.err.println("DISCONNECT FAIL" + e.toString());
		} catch (EOFException e) {
			System.err.println("DISCONNECT FAIL" + e.toString());
		} catch (IOException e) {
			System.err.println("DISCONNECT FAIL" + e.toString());
		} catch (SecurityException e){
			System.err.println("DISCONNECT FAIL" + e.toString());
		} finally {
			/* If succesful disconnect, close the client-server thread*/
			server_Thread.closeConnection();
			try {
				server_Thread.join();
				server_Thread = null;
			} catch (InterruptedException e){
				System.err.println(e.toString());
			}
			/* Set connected to false */
			connected = false;
			connected_user_name = null;
		}
		return 0;
	}

	 /**
	 * Allows the user to create a new file in his directory
	 * @param user - user name
	 * @param file_name    - file name
	 * @param description - descrition
	 */
	static int publish(String user ,String file_name, String description) 
	{
		/* Request to the webservice ToUpperCase method */
		ToUpperCaseServiceService service = new ToUpperCaseServiceService();
		ToUpperCaseService port = service.getToUpperCaseServicePort();
		/* Receive from the webservice the description in upper case */
		String description_capital = port.toUppercase(description);

		/* Transform the command,user_name,file and description to byte arrays*/
		byte [] command = parse_command(COMMANDS.PUBLISH.toString());
		byte [] user_name = parse_string(user);
		byte [] file = parse_string(file_name);
		byte [] description_parsed = parse_string(description_capital);
	
		/* Link the socket to connect with the server and send the request */
		/* Try-resources */
		try (	Socket socket = new Socket(_server,_port);
				DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
				DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());) 
		{
			/* Send command */
			send_stream.write(command);
			send_stream.flush();

			/* Send name of user performing the operation */
			send_stream.write(user_name);
			send_stream.flush();

			/* Send file name */
			send_stream.write(file);
			send_stream.flush();

			/* Send file descpription */
			send_stream.write(description_parsed);
			send_stream.flush();

			/* Recieve the server response */
			int receive_output = receive_Stream.readInt();
			
			switch (receive_output) {
				case 1:
					System.err.println("PUBLISH FAIL, USER NOT REGISTERED");
					break;
				case 2:
					System.err.println("PUBLISH FAIL, USER NOT CONNECTED");
					break;
				case 3:
					System.err.println("PUBLISH FAIL, CONTENT ALREADY PUBLISHED");
					break;
				case 4:
					System.err.println("PUBLISH FAIL");
					break;
				default:
					System.out.println("PUBLISH OK");
			}
		} catch (UnknownHostException e) {
			System.err.println("PUBLISH FAIL" + e.toString());
		} catch (EOFException e) {
			System.err.println("PUBLISH FAIL" + e.toString());
		} catch (IOException e) {
			System.err.println("PUBLISH FAIL" + e.toString());
		} catch (SecurityException e){
			System.err.println("PUBLISH FAIL" + e.toString());
		}  
		return 0;
	}

	 /**
	 * Allows the user to delete a file from his directory specifying the name
	 * 
	 * @param file_name    - file name
	 */
	static int delete(String file_name)
	{
		/* Check if the user is connected */
		if(!connected){
			System.err.println("DELETE FAIL, USER NOT CONNECTED");
			return 0;
		}
		/* Transform the command, user name and file name to byte arrays*/
		byte [] command = parse_command(COMMANDS.DELETE.toString());
		byte [] user_name = parse_string(connected_user_name);
		byte [] file = parse_string(file_name);

		/* Link the socket to connect with the server and send the request */
		/* Try-resources */
		try (	Socket socket = new Socket(_server,_port);
				DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
				DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());) 
		{
			/* Send command */
			send_stream.write(command);
			send_stream.flush();
			/* Send name of user performing the operation */
			send_stream.write(user_name);
			send_stream.flush();
			/* Send name of the file */
			send_stream.write(file);
			send_stream.flush();

			/* Receive the output from server */
			int receive_output = receive_Stream.readInt();
		
			switch (receive_output) {
				case 3:
					System.out.println("DELETE FAIL, CONTENT NOT PUBLISHED");
					break;
				case 4:
					System.out.println("DELETE FAIL");
					break;
				default:
					System.out.println("DELETE OK");
			}

		} catch (UnknownHostException e) {
			System.err.println("PUBLISH FAIL" + e.toString());
		} catch (EOFException e) {
			System.err.println("PUBLISH FAIL" + e.toString());
		} catch (IOException e) {
			System.err.println("PUBLISH FAIL" + e.toString());
		} catch (SecurityException e){
			System.err.println("PUBLISH FAIL" + e.toString());
		}  
		return 0;
	}

	 /**
	 * List all the currently connected users
	 */
	static int list_users()
	{
		/* Check the user calling is connected to the server */
		if(!connected){
			System.err.println("LIST_USERS FAIL / USER NOT CONNECTED");
			return 0;
		}
		/* Transform the command  and user name to byte arrays*/
		byte [] command = parse_command(COMMANDS.LIST_USERS.toString());
		byte [] user_name = parse_string(connected_user_name);
		
		/* Link the socket to connect with the server and send the request */
		/* Try-resources */
		try (	Socket socket = new Socket(_server,_port);
				DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
				DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());) 
		{
			/* Send commmand */
			send_stream.write(command);
			send_stream.flush();
			/* Send name of the user performing the operation */
			send_stream.write(user_name);
			send_stream.flush();
		
			/* Recieve the server response */
			int receive_output = receive_Stream.readInt();

			switch (receive_output) {
				case 3:
					System.out.println("LIST_USERS FAIL");
					break;
			}

			/* Response is success */
			if(receive_output == 0){
				
				/* Try-resources */
				try (BufferedReader receive_data = new BufferedReader(new InputStreamReader(socket.getInputStream())))
				{
					int number_users = receive_Stream.readInt();
					String [] data = new String [3];
					
					System.out.println("LIST_USERS OK");
					
					/* For each user recieved print the recieved data */
					for (int ii = 0; ii < number_users; ii++) {
						for(int jj = 0; jj< 3 ; jj++){
							data[jj] = receive_data.readLine().trim();
						}
						/* Add to the hash map the received users data*/
						users_data.put(data[0], new UsersData(data[1],data[2]));
						System.out.println("     " + data[0] + "  " + data[1] + "  " + data[2]);
					}
					
				} catch (EOFException e) {
					System.err.println("LIST_USERS FAIL" + e.toString());
				} catch (IOException e) {
					System.err.println("LIST_USERS FAIL" + e.toString());
				}
			}
		} catch (UnknownHostException e) {
			System.err.println("LIST_USERS FAIL" + e.toString());
		} catch (EOFException e) {
			System.err.println("LIST_USERS FAIL" + e.toString());
		} catch (IOException e) {
			System.err.println("LIST_USERS FAIL" + e.toString());
		} catch (SecurityException e){
			System.err.println("LIST_USERS FAIL" + e.toString());
		}  
		return 0;
	}

	 /**
	 * Lists the contents of the given user
	 *
	 * @param user_name    - user name
	 */
	static int list_content(String user_name)
	{
		/* Check the user is connected */
		if(!connected){
			System.err.println("LIST_CONTENT FAIL / USER NOT CONNECTED");
			return 0;
		}
		
		/* Transform the command, user_name and, connected user name to byte arrays*/
		byte [] command = parse_command(COMMANDS.LIST_CONTENT.toString());
		byte [] name = parse_string(user_name);
		byte [] userop_name = parse_string(connected_user_name);
		
		/* Link the socket to connect with the server and send the request */
		/* Try-resources */
		try (	Socket socket = new Socket(_server,_port);
				DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
				DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());) 
		{
			/* Send commmand */
			send_stream.write(command);
			send_stream.flush();
			/* Send name of the user performing the operation */
			send_stream.write(userop_name);
			send_stream.flush();
			/* Send name of the user whose content we want*/
			send_stream.write(name);
			send_stream.flush();

			/* Recieve the server response */
			int receive_output = receive_Stream.readInt();

			switch (receive_output) {
				case 3:
					System.out.println("LIST_CONTENT FAIL, REMOTE USER DOES NOT EXIST");
					break;
				case 4:
					System.out.println("LIST_CONTENT FAIL");
					break;
			}
			/* Response is success */
			if (receive_output == 0){
				
				/* Try-resources */
				try (BufferedReader receive_files = new BufferedReader(new InputStreamReader(socket.getInputStream()))) 
				{
					/* Receive the number of files from server */
					int number_files = receive_Stream.readInt();

					/* For each file received output it to the console */
					System.out.println("LIST_CONTENT OK");
					for (int ii = 0; ii < number_files; ii++) {
						System.out.println("     " + receive_files.readLine());
					}
				} catch (EOFException e) {
					System.err.println("LIST_CONTENT FAIL" + e.toString());
				} catch (IOException e) {
					System.err.println("LIST_CONTENT FAIL" + e.toString());
				}
			}

		} catch (UnknownHostException e) {
			System.err.println("LIST_CONTENT FAIL" + e.toString());
		} catch (EOFException e) {
			System.err.println("LIST_CONTENT FAIL" + e.toString());
		} catch (IOException e) {
			System.err.println("LIST_CONTENT FAIL" + e.toString());
		} catch (SecurityException e){
			System.err.println("LIST_CONTENT FAIL" + e.toString());
		} 

		return 0;
	}

	 /**
	 * Returns a file with the name provided and from the user provided
	 * 
	 * @param user_name    - user name
	 * @param remote_file_name    - remote file name
	 * @param local_file_name  - local file name
	 */
	static int get_file(String user_name, String remote_file_name, String local_file_name)
	{
		
		/* Check if the specified user exists in the hash map */
		UsersData remote_Data;
		try {
			remote_Data = users_data.get(user_name);

		} catch (NullPointerException e) {
			System.out.println("GET_FILE FAIL");
			return 0;
		}
		/* If the user does not exist return */
		if(remote_Data == null){
			System.out.println("GET_FILE FAIL");
			return 0;
		}
		/* Link the socket to connect with the client-server and send the request */
		/* Try-resources */
		try (	Socket socket = new Socket(remote_Data.getIp(),Integer.parseInt(remote_Data.getPort()));
				BufferedWriter send_stream = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
				DataInputStream receive_output = new DataInputStream(socket.getInputStream());) 
		{
			/* Send command*/
			send_stream.write(COMMANDS.GET_FILE.toString());
			send_stream.newLine();
			send_stream.flush();
			/* Send the the requested file name */
			send_stream.write(remote_file_name);
			send_stream.newLine();
			send_stream.flush();
			
			/* Recieve the client-server response */
			int response = receive_output.readInt();

			switch(response){
				case 1:
					System.err.println("GET_FILE FAIL / FILE DOES NOT EXIST");
					break;
				case 2:
					System.err.println("GET_FILE FAIL");
					break;
			}
			
			/* No errors */
			if(response == 0){

				/* Construct the file path */
				String file_name = file_path.concat(local_file_name);
				/* Try-resources */
				try (	BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
						BufferedWriter bufferedWriter = new BufferedWriter(new FileWriter(file_name));) 
				{
					/* Create a local file of file name */
					File file = new File(file_name);
					file.createNewFile();
					String line;
					/* Read the contents of the file */
					while((line = bufferedReader.readLine()) != null){
						bufferedWriter.write(line);
						send_stream.newLine();
						bufferedWriter.flush();
					}

					System.out.println("GET_FILE OK");

				} catch (IOException e) {
					System.err.println("GET_FILE FAIL " + e.toString());
				} catch (NullPointerException e) {
					System.err.println("GET_FILE FAIL" + e.toString());
				} catch (SecurityException e) {
					System.err.println("GET_FILE FAIL" + e.toString());
				}
			}

		} catch (UnknownHostException e) {
			System.err.println("GET_FILE FAIL " + e.toString());
		} catch (EOFException e) {
			System.err.println("GET_FILE FAIL " + e.toString());
		} catch (IOException e) {
			/* If the execption is socket closed exception occurs ignore 
				as it will always be thrown when closing BufferedWriter*/
			if(!e.toString().equals("java.net.SocketException: Socket closed")){
				System.err.println("GET_FILE FAIL " + e.toString());
			}
		} catch (SecurityException e){
			System.err.println("GET_FILE FAIL " + e.toString());
		} 
		return 0;
	}

	
	/**
	 * Command interpreter for the client. It calls the protocol functions.
	 */
	static void shell() 
	{
		boolean exit = false;
		String input;
		String [] line;
		BufferedReader in = new BufferedReader(new InputStreamReader(System.in));

		while (!exit) {
			try {
				System.out.print("c> ");
				input = in.readLine();
				line = input.split("\\s");

				if (line.length > 0) {
					/*********** REGISTER *************/
					if (line[0].toUpperCase().equals("REGISTER")) {
						if  (line.length == 2) {
							register(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: REGISTER <userName>");
						}
					} 
					
					/********** UNREGISTER ************/
					else if (line[0].toUpperCase().equals("UNREGISTER")) {
						if  (line.length == 2) {
							unregister(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: UNREGISTER <userName>");
						}
                    			} 
                    			
                    			/************ CONNECT *************/
                    			else if (line[0].toUpperCase().equals("CONNECT")) {
						if  (line.length == 2) {
							connect(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: CONNECT <userName>");
                    				}
                    			} 
                    
                    			/********** DISCONNECT ************/
                    			else if (line[0].toUpperCase().equals("DISCONNECT")) {
						if  (line.length == 2) {
							disconnect(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: DISCONNECT <userName>");
                    				}
                    			} 
                    
                    			/************** PUBLISH **************/
                    			else if (line[0].toUpperCase().equals("PUBLISH")) {
						if  (line.length >= 4) {
							// Remove first two words
							//String description = input.substring(input.indexOf(' ')+1).substring(input.indexOf(' ')+1);
							String description = input.substring(input.indexOf(' ')+3);
							description = description.substring(description.indexOf(' ')+1);
							publish(line[1],line[2], description); // file_name = line[2]
						} else {
							System.out.println("Syntax error. Usage: PUBLISH <userName> <file_name> <description>");
                    				}
                    			} 

                    			/************ DELETE *************/
                    			else if (line[0].toUpperCase().equals("DELETE")) {
						if  (line.length == 2) {
							delete(line[1]);
						} else {
							System.out.println("Syntax error. Usage: DELETE <file name>");
                    				}
                    			} 
                    
                    			/************** LIST_USERS **************/
                    			else if (line[0].toUpperCase().equals("LIST_USERS")) {
						if  (line.length == 1) {
							// Remove first two words
							list_users(); 
						} else {
							System.out.println("Syntax error. Usage: LIST_USERS ");
                    				}
                    			} 
                    
                    			/************ LIST_CONTENT *************/
                    			else if (line[0].toUpperCase().equals("LIST_CONTENT")) {
						if  (line.length == 2) {
							list_content(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: LIST_CONTENT <user name>");
                    				}
                    			} 
                    
                    			/************** GET_FILE **************/
                    			else if (line[0].toUpperCase().equals("GET_FILE")) {
						if  (line.length == 4) {
							get_file(line[1], line[2], line[3]); 
						} else {
							System.out.println("Syntax error. Usage: GET_FILE <user> <remote_file_name> <local_file_name>");
                    				}
                    			} 

                    
                    			/************** QUIT **************/
                    			else if (line[0].toUpperCase().equals("QUIT")){
						if (line.length == 1) {
							exit = true;
						} else {
							System.out.println("Syntax error. Use: QUIT");
						}
					} 
					
					/************* UNKNOWN ************/
					else {						
						System.out.println("Error: command '" + line[0] + "' not valid.");
					}
				}				
			} catch (java.io.IOException e) {
				System.out.println("Exception: " + e);
				e.printStackTrace();
			}
		}
	}
	
	/**
	 * @brief Prints program usage
	 */
	static void usage() 
	{
		System.out.println("Usage: java -cp . client -s <server> -p <port>");
	}
	
	/**
	 * @brief Parses program execution arguments 
	 */ 
	static boolean parseArguments(String [] argv) 
	{
		Getopt g = new Getopt("client", argv, "ds:p:");

		int c;
		String arg;

		while ((c = g.getopt()) != -1) {
			switch(c) {
				//case 'd':
				//	_debug = true;
				//	break;
				case 's':
					_server = g.getOptarg();
					break;
				case 'p':
					arg = g.getOptarg();
					_port = Integer.parseInt(arg);
					break;
				case '?':
					System.out.print("getopt() returned " + c + "\n");
					break; // getopt() already printed an error
				default:
					System.out.print("getopt() returned " + c + "\n");
			}
		}
		
		if (_server == null)
			return false;
		
		if ((_port < 1024) || (_port > 65535)) {
			System.out.println("Error: Port must be in the range 1024 <= port <= 65535");
			return false;
		}

		return true;
	}
	
	
	/********************* MAIN **********************/
	
	public static void main(String[] argv) 
	{
		if(!parseArguments(argv)) {
			usage();
			return;
		}

		
		if(init_client()!=0){
			return;
		}

		shell();
		if(connected){
			server_Thread.closeConnectionQuit(_server,_port);
		}
		System.out.println("+++ FINISHED +++");
	}
}
