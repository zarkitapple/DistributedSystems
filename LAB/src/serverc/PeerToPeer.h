#ifndef _PEERTOPEER_
#define _PEERTOPEER_
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "../../lib/lines.h"
#include "../../lib/LinkedList.h"
#include "../../lib/auxiliary.h"
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>

#define USERS_DIRECTORY "./Users"

/* Definition of possible return values */
enum RETURN_VAL {
    SUCCESS = 0,
    USER_REGISTERED = 1,
    USER_NOT_REGISTERED = 1,
    USER_CONNECTED = 2,
    USER_NOT_CONNECTED = 2,
    FILE_PUBLISHED = 3,
    FILE_NOT_PUBLISHED = 3,
    NO_CONTENT_TO_LIST = 3,
    OTHER_ERROR_2 = 2,
    OTHER_ERROR_3 = 3,
    OTHER_ERROR_4 = 4,
};


/*****************************************************************************
 *                                                                           *
 *  Server functions to implement the peer to peer functionalities           *
 *  specified. These functions either implement an expected functionality    *
 *  of the system directly (CONNECT, DISCONNECT, etc), or act as             *
 *  auxiliary in other client related function's implementation.             *
 *  Client connection is made throught a manager function, which also        *
 *  processes the return values of the following functions, expect for       *
 *  list_contents and list_users.                                            *
 *                                                                           *
 *  Authors:     Pablo Alejandre Blanco                                      *
 *               Mario Pérez Enríquez                                        *
 *                                                                           *
 *  Last Revision Date:        29/04/2020                                    *
 *                                                                           *
 ****************************************************************************/

/* 
 * FUNCTION init_p2p_service initializes the peer to peer service by
 *              creating the data control mechanisms and data structures required
 *              by the service        
 */
int init_p2p_service ();


/* 
 * FUNCTION register_user registers a new user provided it does not exist yet.
 * 
 * ARGS     socket is the socket stablishing connection with the client
 *              requesting the operation to perform. Data streams are read 
 *              from this socket. The number of streams varies according
 *              to the expected inputs of the request.
 * 
 * RETURN   0 If successful registration.
 *          1 If user already registered.
 *          2 In any other case.
 */
int register_user (int socket);

/* 
 * FUNCTION unregister_user unregisters a new user provided it exists.
 * 
 * ARGS     socket is the socket stablishing connection with the client
 *              requesting the operation to perform. Data streams are read 
 *              from this socket. The number of streams varies according
 *              to the expected inputs of the request.
 * 
 * RETURN   0 If successful unregistration.
 *          1 If user is not registered (not exists).
 *          2 In any other case.
 */
int unregister_user (int socket);

/* 
 * FUNCTION connect_user lets a registered user stablish a connection.
 * 
 * ARGS     socket is the socket stablishing connection with the client 
 *              requesting the operation to perform. Data streams are read 
 *              from this socket. The number of streams varies according
 *              to the expected inputs of the request.
 * 
 * RETURN   0 If successful connection.
 *          1 If user is not registered (not exists).
 *          2 If user already connected.
 *          3 In any other case.
 */
int connect_user (int socket);

/* 
 * FUNCTION disconnect_user lets a registered user end a connection.
 * 
 * ARGS     socket is the socket stablishing connection with the client 
 *              requesting the operation to perform. Data streams are read 
 *              from this socket. The number of streams varies according
 *              to the expected inputs of the request.
 * 
 * RETURN   0 If successful disconnection.
 *          1 If user is not registered (not exists).
 *          2 If user not connected.
 *          3 In any other case.
 */
int disconnect_user (int socket);

/* 
 * FUNCTION publish_file lets a registered user publish a file in the server.
 * 
 * ARGS     socket is the socket stablishing connection with the client 
 *              requesting the operation to perform. Data streams are read 
 *              from this socket. The number of streams varies according
 *              to the expected inputs of the request.
 * 
 * RETURN   0 If successful publishment.
 *          1 If user is not registered (not exists).
 *          2 If user not connected.
 *          3 If file already published.
 *          4 In any other case.
 */
int publish_file(int socket);

/* 
 * FUNCTION delete_file lets a registered user delete a file from the server.
 * 
 * ARGS     socket is the socket stablishing connection with the client 
 *              requesting the operation to perform. Data streams are read 
 *              from this socket. The number of streams varies according
 *              to the expected inputs of the request.
 * 
 * RETURN   0 If successful deletion.
 *          1 If user is not registered (not exists).
 *          2 If user not connected.
 *          3 If file not published.
 *          4 In any other case.
 */
int delete_file(int socket);

/* 
 * FUNCTION list_users lists the users that are currently connected.
 * 
 * ARGS     socket is the socket stablishing connection with the client 
 *              requesting the operation to perform. Data streams are read 
 *              from this socket. The number of streams varies according
 *              to the expected inputs of the request.
 * 
 * CLIENT RETURN    
 *                  Exec. Code
 *                      0 If successful listing of users.
 *                      1 If user is not registered (not exists).
 *                      2 If user not connected.
 *                      3 In any other case.
 *                  Data
 *                      List of user datas (name, IP, port).
 */
void list_users (int socket);

/* 
 * FUNCTION list_content lists content uploaded by a user, on request by
 *              another user.
 * 
 * ARGS     socket is the socket stablishing connection with the client 
 *              requesting the operation to perform. Data streams are read 
 *              from this socket. The number of streams varies according
 *              to the expected inputs of the request.
 * 
 * CLIENT RETURN    
 *                  Exec. Code
 *                      0 If successful listing of content.
 *                      1 If user is not registered (not exists).
 *                      2 If not connected.
 *                      3 If no content.
 *                      4 In any other case
 *                  Data
 *                      Files in user directory.
 */
void list_content (int socket);

#endif