package source;
import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Arrays;
import java.util.concurrent.atomic.AtomicBoolean;

import gnu.getopt.Getopt;

class client {
	
	
	/******************* ATTRIBUTES *******************/
	
	private static String _server   = null;
	private static int _port = -1;
	private static final int message_size = 256;
	private static MyThread server_Thread;
	private static AtomicBoolean server_exit = new AtomicBoolean(true);
	enum COMMANDS {
		REGISTER,
		UNREGISTER,
		CONNECT,
		DISCONNECT,
		PUBLISH
	}
	
	/********************* METHODS ********************/
	private static byte [] parse_string (String userString) {
		byte [] byte_array;
		System.out.println(userString.length());
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

	private static byte [] parse_file_name (String file_name){
		byte [] byte_array;
		String file_parsed = file_name.replaceAll("\\s","");
		System.out.println(file_name.length());
		/* truncate string */
		if (file_name.length()>message_size-1){
			System.out.println("Truncate");
			byte_array = file_name.substring(0, message_size).getBytes();
			byte_array[message_size-1] = '\n';
		}
		else {
			System.out.println("Not Truncate");
			file_name = file_name +'\0';
			byte_array = file_name.getBytes();
		}
		System.out.println(Arrays.toString(byte_array));
		return byte_array;
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

		try {
			Socket socket = new Socket(_server,_port);
			DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
			DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());
			send_stream.write(command);
			send_stream.write(user_name);
			int receive_output = Character.getNumericValue(receive_Stream.readByte());
			receive_Stream.close();
			send_stream.close();
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
			send_stream.write(user_name);

			int receive_output = Character.getNumericValue(receive_Stream.readByte());

			receive_Stream.close();
			send_stream.close();
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
		
		byte [] user_name = parse_string(user);
		byte [] command = parse_command(COMMANDS.CONNECT.toString());
		try {
			ServerSocket server_socket = new ServerSocket(0);

			int server_port = server_socket.getLocalPort();
			server_Thread = new MyThread(server_socket,server_exit);
			server_Thread.start();
			Socket socket = new Socket(_server,_port);

			DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());

			DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());


			send_stream.write(command);
			send_stream.write(user_name);

			String port = Integer.toString(server_port) +"\0";
			send_stream.write(port.getBytes());

			int receive_output = Character.getNumericValue(receive_Stream.readByte());

			receive_Stream.close();
			send_stream.close();
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
			server_exit.set(false);
			System.out.println("Finished thread");
			Socket socket = new Socket(_server,_port);
			DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
			DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());

			send_stream.write(command);
			send_stream.write(user_name);

			int receive_output = Character.getNumericValue(receive_Stream.readByte());

			receive_Stream.close();
			send_stream.close();
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
			System.out.println("UNREGISTER FAIL");
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
	static int publish(String file_name, String description) 
	{
		
		byte [] command = parse_command(COMMANDS.UNREGISTER.toString());
		byte [] file = parse_string(file_name);
		byte [] description_parsed = parse_string(description);
		try {
			Socket socket = new Socket(_server,_port);
			DataOutputStream send_stream = new DataOutputStream(socket.getOutputStream());
			DataInputStream receive_Stream = new DataInputStream(socket.getInputStream());

			send_stream.write(command);
			send_stream.write(file);
			send_stream.write(description_parsed);

			int receive_output = Character.getNumericValue(receive_Stream.readByte());

			receive_Stream.close();
			send_stream.close();
			socket.close();
			
			switch (receive_output) {
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
		server_exit.set(false);
		System.out.println("+++ FINISHED +++");
	}
}
