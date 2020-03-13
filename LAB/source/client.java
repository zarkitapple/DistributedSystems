package source;
import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;

import gnu.getopt.Getopt;
import source.MyThread;
class client {
	
	
	/******************* ATTRIBUTES *******************/
	
	private static String _server   = null;
	private static int _port = -1;
	private static final int message_size = 256;
	enum COMMANDS {
		REGISTER,
		UNREGISTER,
		CONNECT
	}
	
	/********************* METHODS ********************/
	private static byte [] parse_string (String userString) {
		byte [] byte_array;
		System.out.println(userString.length());
		/* truncate string */
		if (userString.length()>message_size-1){
			byte_array = userString.substring(0, message_size).getBytes();
			byte_array[message_size-1] = '\n';
		}
		else {
			byte_array = userString.getBytes();
			byte_array[userString.length()]='\n';
		}
		return byte_array;
	}

	private static byte [] parse_command (String type){
		byte [] command = COMMANDS.valueOf(type).toString().getBytes();
		command[command.length] = '\n';
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
		byte [] user_name = parse_string(user);
		byte [] command = parse_command(COMMANDS.REGISTER.toString());
		System.out.println("Byte array");
		for (byte b : user_name) {
			System.out.print(b+"");
		}
		System.out.println();

		try {
			Socket socket = new Socket(_server,_port);
			DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
			DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());
			send_stream.write(command);
			send_stream.write(user_name);

			int receive_output = receive_Stream.readByte();

			receive_Stream.close();
			send_stream.close();
			socket.close();
			
			switch (receive_output) {
				case 1:
					throw new Exception("USER NAME IN USE");
			
				case 2:
					throw new Exception("REGISTER FAIL");
			}

			
		} catch (Exception e) {
			
			System.err.println("Exception"+ e.toString());
			e.printStackTrace();
		}  
		
		System.out.println("REGISTER OK");
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
			send_stream.write(user_name);

			int receive_output = receive_Stream.readByte();

			receive_Stream.close();
			send_stream.close();
			socket.close();
			
			switch (receive_output) {
				case 1:
					throw new Exception("USER DOES NOT EXIST");
			
				case 2:
					throw new Exception("UNREGISTER FAIL");
			}

			
		} catch (Exception e) {
			
			System.err.println("Exception"+ e.toString());
			e.printStackTrace();
		}  
		System.out.println("UNREGISTER OK");
		return 0;
	}
	
    	/**
	 * @param user - User name to connect to the system
	 * 
	 * @return ERROR CODE
	 */
	static int connect(String user) 
	{
		
		byte [] user_name = parse_string(user);
		byte [] command = parse_command(COMMANDS.CONNECT.toString());
		try {
			ServerSocket server_socket = new ServerSocket(0);

			int server_port = server_socket.getLocalPort();
			new MyThread(server_socket).start();

			Socket socket = new Socket(_server,_port);
			DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
			DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());

			send_stream.write(command);
			send_stream.write(user_name);
			send_stream.write(Integer.toString(server_port).getBytes());

			int receive_output = receive_Stream.readByte();

			receive_Stream.close();
			send_stream.close();
			socket.close();
			
			switch (receive_output) {
				case 1:
					throw new Exception("CONNECT FAIL, USER DOES NOT EXIST");
			
				case 2:
					throw new Exception("CONNECT FAIL, USER ALREADY CONNECTED");
			}


		} catch (Exception e) {
			System.err.println("Exception"+ e.toString());
			e.printStackTrace();
		}
		System.out.println("CONNECT OK");
		return 0;
	}
	
	 /**
	 * @param user - User name to disconnect from the system
	 * 
	 * @return ERROR CODE
	 */
	static int disconnect(String user) 
	{
		// Write your code here
		System.out.println("DISCONNECT " + user);
		return 0;
	}

	 /**
	 * @param file_name    - file name
	 * @param description - descrition
	 * 
	 * @return ERROR CODE
	 */
	static int publish(String file_name, String description) 
	{
		// Write your code here
		System.out.println("PUBLISH " + file_name + " " + description);
		return 0;
	}

	 /**
	 * @param file_name    - file name
	 * 
	 * @return ERROR CODE
	 */
	static int delete(String file_name)
	{
		// Write your code here
		System.out.println("DELETE " + file_name);
		return 0;
	}

	 /**
	 * @return ERROR CODE
	 */
	static int list_users()
	{
		// Write your code here
		System.out.println("LIST_USERS " );
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
		System.out.println("LIST_CONTENT " + user_name);
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
		// Write your code here
		System.out.println("GET_FILE " + user_name + " "  + remote_file_name + " " + local_file_name);
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
					if (line[0].equals("REGISTER")) {
						if  (line.length == 2) {
							register(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: REGISTER <userName>");
						}
					} 
					
					/********** UNREGISTER ************/
					else if (line[0].equals("UNREGISTER")) {
						if  (line.length == 2) {
							unregister(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: UNREGISTER <userName>");
						}
                    			} 
                    			
                    			/************ CONNECT *************/
                    			else if (line[0].equals("CONNECT")) {
						if  (line.length == 2) {
							connect(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: CONNECT <userName>");
                    				}
                    			} 
                    
                    			/********** DISCONNECT ************/
                    			else if (line[0].equals("DISCONNECT")) {
						if  (line.length == 2) {
							disconnect(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: DISCONNECT <userName>");
                    				}
                    			} 
                    
                    			/************** PUBLISH **************/
                    			else if (line[0].equals("PUBLISH")) {
						if  (line.length >= 3) {
							// Remove first two words
							//String description = input.substring(input.indexOf(' ')+1).substring(input.indexOf(' ')+1);
							String description = input.substring(input.indexOf(' ')+1);
							description = description.substring(description.indexOf(' ')+1);
							publish(line[1], description); // file_name = line[1]
						} else {
							System.out.println("Syntax error. Usage: PUBLISH <file_name> <description>");
                    				}
                    			} 

                    			/************ DELETE *************/
                    			else if (line[0].equals("DELETE")) {
						if  (line.length == 2) {
							delete(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: DELETE <file name>");
                    				}
                    			} 
                    
                    			/************** LIST_USERS **************/
                    			else if (line[0].equals("LIST_USERS")) {
						if  (line.length == 1) {
							// Remove first two words
							list_users(); 
						} else {
							System.out.println("Syntax error. Usage: LIST_USERS ");
                    				}
                    			} 
                    
                    			/************ LIST_CONTENT *************/
                    			else if (line[0].equals("LIST_CONTENT")) {
						if  (line.length == 2) {
							list_content(line[1]); // userName = line[1]
						} else {
							System.out.println("Syntax error. Usage: LIST_CONTENT <user name>");
                    				}
                    			} 
                    
                    			/************** GET_FILE **************/
                    			else if (line[0].equals("GET_FILE")) {
						if  (line.length == 4) {
							get_file(line[1], line[2], line[3]); 
						} else {
							System.out.println("Syntax error. Usage: GET_FILE <user> <remote_file_name> <local_file_name>");
                    				}
                    			} 

                    
                    			/************** QUIT **************/
                    			else if (line[0].equals("QUIT")){
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
		
		// Write code here
		
		shell();
		System.out.println("+++ FINISHED +++");
	}
}
