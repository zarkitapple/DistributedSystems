package src.clientjava;

import src.clientjava.touppercase.ToUpperCaseServiceService;
import src.clientjava.touppercase.ToUpperCaseService;
import src.clientjava.UsersData;
import java.io.*;
import java.lang.reflect.Array;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Arrays;
import java.util.Iterator;
import java.util.concurrent.*;


import gnu.getopt.Getopt;

class client {
	
	
	/******************* ATTRIBUTES *******************/
	
	private static String _server = null;
	private static int _port = -1;
	private static final int message_size = 256;
	private static byte [] _user = null;
	private static boolean connected = false;
	private static MyThread server_Thread = null;
	private static ConcurrentHashMap<String, UsersData> users_data = new ConcurrentHashMap<String, UsersData>();
	private static String file_path = "./UserFiles/";  

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
	
	/********************* METHODS ********************/
	private static byte [] parse_string (String userString) {
		byte [] byte_array;
		System.out.println("String lenght  " +userString.length());
		/* truncate string */
		if (userString.length()>message_size-1){
			System.out.println("Truncate");
			byte_array = userString.substring(0, message_size).getBytes();
			byte_array[message_size-1] = '\n';
		}
		else {
			System.out.println("Not Truncate");
			userString = userString +'\0';
			byte_array = userString.getBytes();
		}
		System.out.println(Arrays.toString(byte_array));
		return byte_array;
	}

	private static byte [] parse_command (String type){
		String command_string = COMMANDS.valueOf(type).toString();
		command_string = command_string + '\0';
		byte [] command = command_string.getBytes();
		System.out.println(Arrays.toString(command));
		return command;
	}
	/**
	 * @param user - User name to register in the system
	 * 
	 * @return ERROR CODE
	 */
	static int register(String user) 
	{	
		// Write your code here
		System.out.println("********************************************");
		byte [] user_name = parse_string(user);
		byte [] command = parse_command(COMMANDS.REGISTER.toString());

		try {
			Socket socket = new Socket(_server,_port);
			
			DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
			DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());

			send_stream.write(command);
			send_stream.flush();
			send_stream.write(user_name);
			send_stream.flush();
			

			int receive_output = receive_Stream.readInt();

			send_stream.close();
			receive_Stream.close();
			socket.close();
			
			switch (receive_output) {
				case 1:
					System.out.println("USER NAME IN USE");
					break;
				case 2:
					System.out.println("REGISTER FAIL");
					break;
				default:
					System.out.println("REGISTER OK");
			}

			
		} catch (Exception e) {
			System.out.println("REGISTER FAIL");
			System.err.println("Exception"+ e.toString());
			e.printStackTrace();
		}  
		return 0;
	}
	
	/**
	 * @param user - User name to unregister from the system
	 * 
	 * @return ERROR CODE
	 */
	static int unregister(String user) 
	{
		byte [] user_name = parse_string(user);
		byte [] command = parse_command(COMMANDS.UNREGISTER.toString());
		
		try {
			Socket socket = new Socket(_server,_port);

			DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
			DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());

			send_stream.write(command);
			send_stream.flush();
			send_stream.write(user_name);
			send_stream.flush();

			int receive_output = receive_Stream.readInt();
			
			send_stream.close();
			receive_Stream.close();
			socket.close();
			
			switch (receive_output) {
				case 1:
					System.out.println("USER DOES NOT EXIST");
					break;
				case 2:
					System.out.println("UNREGISTER FAIL");
					break;
				default:
					System.out.println("UNREGISTER OK");
			}

			
		} catch (Exception e) {
			System.out.println("UNREGISTER FAIL");
			System.err.println("Exception"+ e.toString());
			e.printStackTrace();
		}  
		return 0;
	}
	
    	/**
	 * @param user - User name to connect to the system
	 * 
	 * @return ERROR CODE
	 */
	static int connect(String user) 
	{
		if(connected){
			System.out.println("CONNECT FAIL, USER ALREADY CONNECTED");
			return 0;
		}
		byte [] user_name = parse_string(user);
		byte [] command = parse_command(COMMANDS.CONNECT.toString());
		try {

			ServerSocket server_socket = new ServerSocket(0);
			int server_port = server_socket.getLocalPort();

			server_Thread = new MyThread(server_socket,user_name,users_data);
			server_Thread.start();

			Socket socket = new Socket(_server,_port);
			
			DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
			DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());


			send_stream.write(command);
			send_stream.flush();

			send_stream.write(user_name);
			send_stream.flush();

			String port = Integer.toString(server_port) +"\0";
			send_stream.write(port.getBytes());
			send_stream.flush();

			int receive_output = receive_Stream.readInt();

			send_stream.close();
			receive_Stream.close();
			socket.close();

			switch (receive_output) {
				case 1:
					System.out.println("CONNECT FAIL, USER DOES NOT EXIST");
					break;
				case 2:
					System.out.println("CONNECT FAIL, USER ALREADY CONNECTED");
					break;
				case 3:
					System.out.println("CONNECT FAIL");
					break;
				default:
					System.out.println("CONNECT OK");
			}


		} catch (Exception e) {
			System.out.println("CONNECT FAIL");
			System.err.println("Exception"+ e.toString());
			e.printStackTrace();
		}
		return 0;
	}
	
	 /**
	 * @param user - User name to disconnect from the system
	 * 
	 * @return ERROR CODE
	 */
	static int disconnect(String user) 
	{
		byte [] user_name = parse_string(user);
		byte [] command = parse_command(COMMANDS.DISCONNECT.toString());
		
		try {
			if(server_Thread!=null){
				server_Thread.closeConnectionManually();
			}

			Socket socket = new Socket(_server,_port);

			DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
			DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());

			send_stream.write(command);
			send_stream.flush();
			send_stream.write(user_name);
			send_stream.flush();

			int receive_output = receive_Stream.readInt();

			send_stream.close();
			receive_Stream.close();
			socket.close();
			
			switch (receive_output) {
				case 1:
					System.out.println("DISCONNECT FAIL / USER DOES NOT EXIST");
					break;
				case 2:
					System.out.println("DISCONNECT FAIL / USER NOT CONNECTED");
					break;
				case 3:
					System.out.println("DISCONNECT FAIL");
					break;
				default:
					System.out.println("DISCONNECT OK");
			}

			
		} catch (Exception e) {
			System.out.println("DISCONNECT FAIL");
			System.err.println("Exception"+ e.toString());
			e.printStackTrace();
		}  
		return 0;
		
	}

	 /**
	 * @param file_name    - file name
	 * @param description - descrition
	 * 
	 * @return ERROR CODE
	 */
	static int publish(String user ,String file_name, String description) 
	{
		System.out.println("User     "+user);
		System.out.println("file    "+file_name);
		System.out.println("des   "+description);

		ToUpperCaseServiceService service = new ToUpperCaseServiceService();
		ToUpperCaseService port = service.getToUpperCaseServicePort();

		String descriptio_capital = port.toUppercase(description);

		System.out.println("To capital" + descriptio_capital);

		byte [] command = parse_command(COMMANDS.PUBLISH.toString());
		byte [] user_name = parse_string(user);
		byte [] file = parse_string(file_name);
		byte [] description_parsed = parse_string(descriptio_capital);
		try {
			Socket socket = new Socket(_server,_port);

			DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
			DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());

			send_stream.write(command);
			send_stream.flush();

			send_stream.write(user_name);
			send_stream.flush();

			send_stream.write(file);
			send_stream.flush();

			send_stream.write(description_parsed);
			send_stream.flush();

			int receive_output = receive_Stream.readInt();
	
			send_stream.close();
			receive_Stream.close();
			socket.close();
			
			switch (receive_output) {
				case 1:
					System.out.println("PUBLISH FAIL, USER NOT REGISTERED");
					break;
				case 2:
					System.out.println("PUBLISH FAIL, USER NOT CONNECTED");
					break;
				case 3:
					System.out.println("PUBLISH FAIL, CONTENT ALREADY PUBLISHED");
					break;
				case 4:
					System.out.println("PUBLISH FAIL");
					break;
				default:
					System.out.println("PUBLISH OK");
			}

			
		} catch (Exception e) {
			System.out.println("PUBLISH FAIL");
			System.err.println("Exception"+ e.toString());
			e.printStackTrace();
		}  
		return 0;
	}

	 /**
	 * @param file_name    - file name
	 * 
	 * @return ERROR CODE
	 */
	static int delete(String user,String file_name)
	{
		byte [] command = parse_command(COMMANDS.DELETE.toString());
		byte [] user_name = parse_string(user);
		byte [] file = parse_string(file_name);
		try {
			Socket socket = new Socket(_server,_port);

			DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
			DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());

			send_stream.write(command);
			send_stream.flush();

			send_stream.write(user_name);
			send_stream.flush();

			send_stream.write(file);
			send_stream.flush();

			int receive_output = receive_Stream.readInt();
			
			send_stream.close();
			receive_Stream.close();
			socket.close();
			
			switch (receive_output) {
				case 1:
					System.out.println("DELETE FAIL, USER NOT EXISTS");
					break;
				case 2:
					System.out.println("DELETE FAIL, USER NOT CONNECTED");
					break;
				case 3:
					System.out.println("DELETE FAIL, CONTENT NOT PUBLISHED");
					break;
				case 4:
					System.out.println("DELETE FAIL");
					break;
				default:
					System.out.println("DELETE OK");
			}

			
		} catch (Exception e) {
			System.out.println("DELETE FAIL");
			System.err.println("Exception"+ e.toString());
			e.printStackTrace();
		}  
		return 0;
	}

	 /**
	 * @return ERROR CODE
	 */
	static int list_users()
	{
		byte [] command = parse_command(COMMANDS.LIST_USERS.toString());
		try {
			Socket socket = new Socket(_server,_port);

			DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
			DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());

			send_stream.write(command);
			send_stream.flush();
		
			int receive_output = receive_Stream.readInt();
			System.out.println("output " + receive_output);

			switch (receive_output) {
				case 1:
					System.out.println("LIST_USERS FAIL, USER DOES NOT EXIST");
					socket.close();
					return 0;
				case 2:
					System.out.println("LIST_USERS FAIL, USER NOT CONNECTED");
					socket.close();
					return 0;
				case 3:
					System.out.println("LIST_USERS FAIL");
					socket.close();
					return 0;
			}

			int number_users = receive_Stream.readInt();
			String [] data = new String [3];
			BufferedReader receive_data = new BufferedReader(new InputStreamReader(socket.getInputStream()));
			System.out.println("LIST_USERS OK");
			for (int ii = 0; ii < number_users; ii++) {
				for(int jj = 0; jj< 3 ; jj++){
					data[jj] = receive_data.readLine().trim();
				}
				users_data.put(data[0], new UsersData(data[1],data[2]));
				System.out.println("     " + data[0] + "  " + data[1] + "  " + data[2]);
			}
			receive_data.close();
			send_stream.close();
			receive_Stream.close();
			socket.close();

		} catch (Exception e) {
			System.out.println("LIST_USERS FAIL");
			System.err.println("Exception "+ e.toString());
			e.printStackTrace();
			
		}  
		return 0;
	}


	 /**
	 * @param user_name    - user name
	 * 
	 * @return ERROR CODE
	 */
	static int list_content(String user_name)
	{
		// Write your code here
		byte [] command = parse_command(COMMANDS.LIST_CONTENT.toString());
		byte [] name = parse_string(user_name);
		try {
			Socket socket = new Socket(_server,_port);

			DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
			DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());

			send_stream.write(command);
			send_stream.flush();
			send_stream.write(name);
			send_stream.flush();

			int receive_output = receive_Stream.readInt();
			System.out.println("output " + receive_output);

			switch (receive_output) {
				case 1:
					System.out.println("LIST_CONTENT FAIL, USER DOES NOT EXIST");
					return 0;
				case 2:
					System.out.println("LIST_CONTENT FAIL, USER NOT CONNECTED");
					return 0;
				case 3:
					System.out.println("LIST_CONTENT FAIL, REMOTE USER DOES NOT EXIST");
					return 0;
				case 4:
					System.out.println("LIST_CONTENT FAIL");
					return 0;
			}
			int number_files = receive_Stream.readInt();
			

			BufferedReader receive_files = new BufferedReader(new InputStreamReader(socket.getInputStream()));
			System.out.println("LIST_CONTENT OK");
			for (int ii = 0; ii < number_files; ii++) {
				System.out.println(receive_files.readLine());
			}
	
			receive_files.close();
			send_stream.close();
			receive_Stream.close();
			socket.close();

		} catch (Exception e) {
			System.out.println("LIST_CONTENT FAIL");
			System.err.println("Exception "+ e.toString());
			e.printStackTrace();
			
		}  
		return 0;
	}

	 /**
	 * @param user_name    - user name
	 * @param remote_file_name    - remote file name
	 * @param local_file_name  - local file name
	 * 
	 * @return ERROR CODE
	 */
	static int get_file(String user_name, String remote_file_name, String local_file_name)
	{
		
		try {
			
			UsersData remote_Data = users_data.get(user_name);
			if (remote_Data == null) {
				System.out.println("GET_FILE FAIL");
				return 0;
			}

			Socket socket = new Socket(remote_Data.getIp(),Integer.parseInt(remote_Data.getPort()));

			BufferedWriter send_stream = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));


			System.out.println(COMMANDS.GET_FILE.toString());

			send_stream.write(COMMANDS.GET_FILE.toString());
			send_stream.newLine();
			send_stream.flush();

			send_stream.write(remote_file_name);
			send_stream.newLine();
			send_stream.flush();

			DataInputStream receive_output = new DataInputStream(socket.getInputStream());
			int response = receive_output.readInt();
			switch(response){
				case 1:
					System.out.println("GET_FILE FAIL / FILE DOES NOT EXIST");
					socket.close();
					return 0;
				case 2:
					System.out.println("GET_FILE FAIL");
					socket.close();
					return 0;
			}
			String file_name = file_path.concat(local_file_name);
			File file = new File(file_name);

			file.createNewFile();
			BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
			BufferedWriter bufferedWriter = new BufferedWriter(new FileWriter(file_name));
			String line;

			while((line = bufferedReader.readLine()) != null){
				bufferedWriter.write(line);
				send_stream.newLine();
				bufferedWriter.flush();
			}

			System.out.println("GET_FILE OK");

			bufferedWriter.close();
			socket.close();


		
		} catch (Exception e) {
			System.err.println(e.toString());
			e.printStackTrace();
		}
		return 0;
	}

	
	/**
	 * @brief Command interpreter for the client. It calls the protocol functions.
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
						if  (line.length == 3) {
							delete(line[1],line[2]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: DELETE <userName> <file name>");
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
		
		shell();
		if(server_Thread != null){
			server_Thread.closeConnectionAuto(_server,_port);
		}
		System.out.println("+++ FINISHED +++");
	}
}
