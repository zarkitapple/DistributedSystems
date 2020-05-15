#ifndef _AUXILIARY_
#define __AUXILIARY__

#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>

/*****************************************************************************
 *                                                                           *
 *  Auxiliary functions used for the requested functionalities. These are    *
 *  used for both the socket connection based p2p and the RPC based p2p.     *
 *  These function are server specific so no clients can access them.        *
 *                                                                           *
 *  Authors:     Pablo Alejandre Blanco                                      *
 *               Mario Pérez Enríquez                                        *
 *                                                                           *
 *  Last Revision Date:        17/05/2020                                    *
 *                                                                           *
 ****************************************************************************/

#define USERS_DIRECTORY "./Users"

/* 
 * FUNCTION isusr_registered checks if a user is registered in the server.
 * 
 * ARGS     user_name is the name of the user that is checked to be registered.
 * 
 * RETURN   0 If user registered.
 *          1 If user not registered.
 */
int isusr_registered(char * user_name);

/* 
 * FUNCTION remove_directory removes a (user) directory.
 * 
 * ARGS     path is the path in the server to the directory to remove.
 * 
 * RETURN   0 If successfully removed.
 *          -1 If directory not exists.
 */
int remove_directory(const char *path);

/* 
 * FUNCTION file_exists checks a file exists given a path to a directory.
 * 
 * ARGS     file_name is the name of the file to check.
 *          directory_path is the relative path of the directory to search.
 * 
 * RETURN   1 If file exists.
 *          0 If file not exists.
 */
int file_exists (char * file_name, char * directory_path);

/* 
 * FUNCTION get_socket_address returns the address of the specified socket.
 * 
 * ARGS     socket is the socket whose address is unknown.
 * 
 * RETURN   NULL if error or no such socket.
 *          char * ADDRESS if success.
 */
char * get_socket_address(int socket);

#endif